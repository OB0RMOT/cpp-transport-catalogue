#include "request_handler.h"
#include "json_reader.h"

using namespace std;
using namespace transport_catalogue;
using namespace json;
using namespace renderer;

int main() {
    /*try {
        // Ваш код, который может вызвать исключение
        throw std::logic_error("Not an array");
    }
    catch (const std::exception& e) {
        std::cerr << "Исключение: " << e.what() << std::endl;

        // Воспользуемся макросом __FILE__ и __LINE__ для вывода места, где произошло исключение
        std::cerr << "In file " << __FILE__ << ", string " << __LINE__ << std::endl;

        // Если вы компилируете с отладочной информацией, gdb может предоставить подробный трейс стека
        // Запустите программу в gdb: gdb ./ваша_программа
        // Затем введите команду run и программа выполнится до исключения
        // Когда произойдет исключение, используйте команду bt (backtrace) для получения трейса стека
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