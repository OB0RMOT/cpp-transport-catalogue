#include "request_handler.h"
#include "json_reader.h"

using namespace std;
using namespace transport_catalogue;
using namespace json;
using namespace renderer;

int main() {
    /*try {
        // ��� ���, ������� ����� ������� ����������
        throw std::logic_error("Not an array");
    }
    catch (const std::exception& e) {
        std::cerr << "����������: " << e.what() << std::endl;

        // ������������� �������� __FILE__ � __LINE__ ��� ������ �����, ��� ��������� ����������
        std::cerr << "In file " << __FILE__ << ", string " << __LINE__ << std::endl;

        // ���� �� ������������ � ���������� �����������, gdb ����� ������������ ��������� ����� �����
        // ��������� ��������� � gdb: gdb ./����_���������
        // ����� ������� ������� run � ��������� ���������� �� ����������
        // ����� ���������� ����������, ����������� ������� bt (backtrace) ��� ��������� ������ �����
    }*/
    TransportCatalogue tcat;
    JsonReader jread(Load(cin));
    MapRenderer renderer(jread.LoadRenderSettings());
    jread.FillTransportCatalogue(tcat);
    Router router(jread.GetRoutingSettings(), tcat);
    RequestHandler rhand(tcat, router, renderer);
    jread.ProcessStatRequests(rhand, cout);
    
}/*
#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"

using namespace std;
using namespace transport_catalogue;
using namespace json;

int main() {
    TransportCatalogue tcat;
    JsonReader jread(Load(cin));
    jread.FillTransportCatalogue(tcat);
    jread.LoadRenderSettings().GetSvgDocument(tcat.GetBuses()).Render(cout);
}
*/