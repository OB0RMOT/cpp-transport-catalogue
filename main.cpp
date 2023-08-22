#include <iostream>
#include <sstream>

#include "input_reader.h"


using namespace std; 

int main() {
    transport_catalogue::TransportCatalogue transport_catalogue;
    transport_catalogue::input_reader::LoadQueries(std::cin, transport_catalogue);
}