#include <iostream>
#include <sstream>

#include "input_reader.h"

using namespace std;

int main()
{
    transport_catalogue::TransportCatalogue transport_catalogue;
    vector<pair<char, string>> output_queries = transport_catalogue::input_reader::LoadQueries(cin, transport_catalogue);
    cout << transport_catalogue::stat_reader::GetInfoOnQueries(output_queries, transport_catalogue).str();
}