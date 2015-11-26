#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <iomanip>

enum line_type {t_unknown, t_header, t_reverse_flow, t_turb_visc_limited, t_empty};

line_type get_line_type(std::string line) {
    line_type t_line;
    if (line.substr(0, 6) == "  iter") {
        t_line = t_header;
    } else if (line.substr(0, 14) == " reversed flow") {
        t_line = t_reverse_flow;
    } else if (line.substr(0, 28) == " turbulent viscosity limited") {
        t_line = t_turb_visc_limited;
    } else 
        t_line = t_unknown;
    return t_line;
}

std::vector<double> parse_items(std::string line) {
    double item;
    std::vector<double> data;
    std::stringstream sstr;
    sstr << line;
    while (sstr.good()) {
        sstr >> item;
        data.push_back(item);
    }
    return data;
}

std::vector<std::string> parse_header(std::string line) {
    std::string item;
    std::vector<std::string> header;
    std::stringstream sstr;
    sstr << line;
	if (line.substr(0, 9) == "/solve it") {
		for (unsigned i = 0; i < 3; ++i)
			sstr >> item;
	} else if (line.substr(0, 39) == "hybrid initialization is done./solve it") {
		for (unsigned i = 0; i < 6; ++i)
			sstr >> item;
	}
    while (sstr.good()) {
        sstr >> item;
        header.push_back(item);
    }
    return header;
}

std::vector<double> read_file(std::string filename) {
    std::cout << filename << std::endl;
    std::ifstream file(filename, std::ios::in);
	std::ofstream outfile2;
	outfile2.open(filename + "_cleaned.txt", std::ios::out);
    std::vector<std::string> header;
    std::vector<double> data;
    std::regex regex_inlet("pressure-inlet");
    std::regex regex_outlet("pressure-outlet");
    std::smatch match;
    if (file.is_open()) {
        line_type t_line;
        bool has_reverse_flow_inlet = false;
        bool has_reverse_flow_outlet = false;
        bool has_turb_visc_limited = false;
        t_line = t_empty;
        bool data_part = false;
        // Skip header
        for (std::string line_string; std::getline(file, line_string); ) {
            line_string.erase( std::remove( line_string.begin(), line_string.end(), '\r' ), line_string.end() );
            if (!line_string.empty()){
                if ((!data_part) && ((line_string.substr(0, 9) == "/solve it") ||  (line_string.substr(0, 39) == "hybrid initialization is done./solve it"))) {
					std::cout << line_string << std::endl;
					data_part = true;
					if (header.empty()) {
                        header = parse_header(line_string);
						outfile2 << line_string << std::endl;
                    }
				} else if (data_part) {
                    if (line_string.substr(0, 21) == "/file write-case-data")
                        break;
					else if (line_string.substr(0, 23) == "> /file write-case-data")
						break;
                    if (t_line == t_unknown) {
                        has_reverse_flow_inlet = false;
                        has_reverse_flow_outlet = false;
                        has_turb_visc_limited = false;
                    }
                    t_line = get_line_type(line_string);
                    switch (t_line) {
                        case t_turb_visc_limited :
                            has_turb_visc_limited = true;
                            break;
                        case t_reverse_flow :
                            if (std::regex_search(line_string, regex_inlet)) {
                                has_reverse_flow_inlet = true;
                            }
                            if (std::regex_search(line_string, regex_outlet))
                                has_reverse_flow_outlet = true;
                            break;
                        case t_unknown :
							outfile2 << line_string << std::endl;
                            data = parse_items(line_string);
                            //for (auto dat : data)
                                //std::cout << std::fixed << std::setprecision(6) << dat << " ";
                            //std::cout << std::endl;
                            break;
                        case t_header :
                            if (header.empty()) {
                                header = parse_header(line_string);
								outfile2 << line_string << std::endl;
                            }
                        case t_empty : ;
                    }
                }
            }
        }
        std::cout << "has_reverse_flow_inlet: " << has_reverse_flow_inlet << "; has_reverse_flow_outlet: " << has_reverse_flow_outlet << "; has_turb_visc_limited: " << has_turb_visc_limited << std::endl;
		outfile2 << "has_reverse_flow_inlet: " << has_reverse_flow_inlet << "; has_reverse_flow_outlet: " << has_reverse_flow_outlet << "; has_turb_visc_limited: " << has_turb_visc_limited << std::endl;
        file.close();
		outfile2.close();
    }
	
    return data;
}

int main(int argc, char **argv) {
    std::vector<std::string> infiles;
    for (int i = 1; i < argc; ++i) {
        infiles.push_back(argv[i]);
    }
    char delim = ';';
    double length = 0.8;
    double rho = 1.225;
    double Durchmesser = 0.8;
    double b_E = 1.25;
    double b_A = 0.5;
    int column = 5;
    std::regex regex_pressure ("([0-9]+)Pa");
    std::regex regex_rpm ("([0-9]+)rpm");
    std::regex regex_testing_nr("(RV[0-9]+)");
    std::smatch str_match;
    std::string testing_nr;
    if (std::regex_search(infiles.front(), str_match, regex_testing_nr))
        testing_nr = str_match[1];
    std::ofstream outfile;
    outfile.open(testing_nr + "_results.txt", std::ios::out);
    outfile << "Länge" << delim << "rho" << delim << "Durchmesser" << delim << "b_E / m" << delim << "b_A / m"<< std::endl;
    outfile << length << delim << rho << delim << Durchmesser << delim << b_E << delim << b_A << "\n" << std::endl;
    outfile << "Gauge Pressure" << delim << "Drehzahl" << delim << "Anzahl Iter" << delim << "continuity" << delim << "x-velocity" << delim << "y-velocity"<< delim << "k" << delim << "epsilon" << delim
            << "Cm-1" << delim << "V\u0307 pro meter" << delim << "V\u0307" << delim << "pst_E" << delim << "pt_E" << delim << "pt_E(pst_E+pd_E)" << delim << "pst_A" << delim << "pt_A" << delim
            << "\u0394p_fa" << delim << "\u0394p_t(\u0394p_fa+pd_A)" << delim << "\u0394p_t" << delim 
            << "u2" << delim << "Lieferzahl" << delim << "Druckzahl" << delim << "Druckzahl_fa" << delim << std::endl;
    for (auto file : infiles) {
        std::string pressure, rpm;
        double gauge_pressure = 0;
        double drehzahl = 0;
        if (std::regex_search(file, str_match, regex_pressure))
            pressure = str_match[1];
        if (!pressure.empty())
            gauge_pressure = std::stod(pressure);
        if (std::regex_search(file, str_match, regex_rpm))
            rpm = str_match[1];
        if (!rpm.empty())
            drehzahl = std::stod(rpm);
        std::vector<double> data = read_file(file);
        outfile << gauge_pressure << delim << drehzahl << delim;
		if (data.size() > 2) {
			for (unsigned int i = 0; i < data.size() - 2; ++i) {
				outfile << data[i] << delim;
				//std::cout << "test" << std::endl;
				if (i == data.size() - 7)
					outfile << "=-J" << column << "*$A$2" << delim;
                else if (i == data.size() - 5)
                    outfile << "=-L" << column << "-$B$2/2*(J" << column << "/ $D$2)^2" << delim;
			}
		} else {
			std::cout << "size: " << data.size() << " " << data[0] << std::endl;
		}
        outfile << "=-M" << column << delim;
        outfile << "=Q" << column << "+$B$2/2*(J" << column << "/ $E$2)^2" << delim;
        outfile << "=P" << column << "-M" << column << delim;
        outfile << "=$C$2*PI()*B" << column << "/60" << delim;
        outfile << "=k" << column << "/(T" << column << "*$C$2*$A$2)" << delim;
        outfile << "=R" << column << "/($B$2/2*T" << column << "^2)" << delim;
        outfile << "=Q" << column << "/($B$2/2*T" << column << "^2)" << delim;
        outfile << std::endl;
        column++;
    }
    outfile.close();
    return 0;
}
