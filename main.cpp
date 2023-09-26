#include "request_handler.h"
#include "json_reader.h"

using namespace std;
using namespace transport_catalogue;
using namespace json;
using namespace renderer;

int main() {
    TransportCatalogue tcat;
    JsonReader jread(Load(cin));
    MapRenderer renderer(jread.LoadRenderSettings());
    jread.FillTransportCatalogue(tcat);
    RequestHandler rhand(tcat, renderer);
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