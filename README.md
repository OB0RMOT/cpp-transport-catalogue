# cpp-transport-catalogue

Транспортный справочник.  
- Принимает на вход данные JSON-формата и выдает ответ в виде SVG-файла, визуализирующего остановки и маршруты.  
- Находит кратчайший маршрут между остановками. 
- Для ускорения вычислений сделана сериализация базы справочника через Google Protobuf. 
- Реализован конструктор JSON, позволяющий находить неправильную последовательность методов на этапе компиляции.  
---
## Инструкция по сборке проекта (Visual Studio)
1. Установить Google Protobuf. Скачать с [официального репозитория](https://github.com/protocolbuffers/protobuf/releases) архив protobuf-cpp и распаковать его на компьютере.
2. Создать папки build-debug и build-release для сборки двух конфигураций Protobuf. Предварительно создать папку package, в которую будет складываться пакет Protobuf.
3. Собрать и установить проект (в примере сборка Debug) следующими командами:
```
cmake path\protobuf-3.15.8\cmake -DCMAKE_SYSTEM_VERSION=10.0.17763.0 -DCMAKE_BUILD_TYPE=Debug \ -Dprotobuf_BUILD_TESTS=OFF 
\ -DCMAKE_INSTALL_PREFIX=path\protobuf-3.15.8\package
cmake --build .
cmake --install . 
```
4. В папке package появился bin\protoc.exe - с помощью него будут копилироваться proto-файлы, а в папке lib - статические библиотеки для работы с Protobuf.
5. Для компиляции proto-файла нужно выполнить следующую команду:  
`<путь к пакету Protobuf>\bin\proto --cpp_out . transport_catalogue.proto`  
6. Собрать проект с помощью CMake:
```
cmake . -DCMAKE_PREFIX_PATH=/path/to/protobuf/package
cmake --build .
```
7. При необходимости добавить папки include и lib в дополнительные зависимости проекта - Additional Include Directories и Additional Dependencies.
---
## Запуск программы
Для создания базы транспортного справочника и ее сериализации в файл по запросам base_requests необходимо запустить программу с параметром make_base, указав при этом входной JSON-файл.  
Пример запуска программы для заполнения базы:  
`transport_catalogue.exe make_base <base.json`

Для того, чтобы использовать полученную базу и десериализовать ее для ответов на запросы stat_requests нужно запустить программу с параметром process_requests, указав входной JSON-файл, содержащий запросы к БД и выходной файл, который будет содержать ответы на запросы.  
Пример запуска программы для выполнения запросов к базе:  
`transport_catalogue.exe process_requests <req.json >out.txt`

---
## Формат входных данных
Входные данные поступают программе из stdin в формате JSON-объекта, который имеет на верхнем уровне следующую структуру:  
```
{
  "base_requests": [ ... ],
  "render_settings": { ... },
  "routing_settings": { ... },
  "serialization_settings": { ... },
  "stat_requests": [ ... ]
}
```  
Это словарь, содержащий ключи:  
`base_requests` — массив с описанием автобусных маршрутов и остановок.  
`stat_requests` — массив с запросами к транспортному справочнику.  
`render_settings` — словарь для отрисовки изображения.  
`routing_settings` — словарь, содержащий в себе настройки для скорости автобусов и времени ожидания на остановке.  
`serialization_settings` — настройки сериализации.

---

### Заполнение базы транспортного справочника

#### Сериализация базы данных
В ключе file указывается название файла, из которого нужно считать сериализованную базу.
```
      "serialization_settings": {
          "file": "transport_catalogue.db"
      }
```

#### Пример описания остановки:  
```
{
  "type": "Stop",
  "name": "Электросети",
  "latitude": 43.598701,
  "longitude": 39.730623,
  "road_distances": {
    "Улица Докучаева": 3000,
    "Улица Лизы Чайкиной": 4300
  }
} 
```
Описание остановки — словарь с ключами:  
`type` — строка, равная "Stop", означает, что объект описывает остановку;  
`name` — название остановки;  
`latitude` и `longitude` задают координаты широты и долготы остановки;  
`road_distances` — словарь, задающий расстояние до соседних остановок. Ключ — название остановки, значение — целое число в метрах.  
#### Пример описания автобусного маршрута:  
```
{
  "type": "Bus",
  "name": "14",
  "stops": [
    "Улица Лизы Чайкиной",
    "Электросети",
    "Улица Докучаева",
    "Улица Лизы Чайкиной"
  ],
  "is_roundtrip": true
} 
```
Описание автобусного маршрута — словарь с ключами:  
`type` — строка"Bus", означающая, что объект описывает автобусный маршрут;  
`name` — название маршрута;  
`stops` — массив с названиями остановок, через которые проходит автобусный маршрут. У кольцевого маршрута название последней остановки дублирует название первой. Например: `["stop1", "stop2", "stop3", "stop1"]`;  
`is_roundtrip` — значение типа bool. Указывает, кольцевой маршрут или нет.  

#### Структура словаря render_settings:
```
{
  "width": 1200.0,
  "height": 1200.0,

  "padding": 50.0,

  "line_width": 14.0,
  "stop_radius": 5.0,

  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],

  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],

  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,

  "color_palette": [
    "green",
    [255, 160, 0],
    "red"
  ]
} 
```
`width` и `height` — ключи, которые задают ширину и высоту в пикселях. Вещественное число в диапазоне `от 0 до 100000`.  
`padding` — отступ краёв карты от границ `SVG`-документа. Вещественное число не меньше 0 и меньше `min(width, height)/2`.  
`line_width` — толщина линий, которыми рисуются автобусные маршруты. Вещественное число в диапазоне `от 0 до 100000`.  
`stop_radius` — радиус окружностей, которыми обозначаются остановки. Вещественное число в диапазоне `от 0 до 100000`.  
`bus_label_font_size` — размер текста, которым написаны названия автобусных маршрутов. Целое число в диапазоне `от 0 до 100000`.  
`bus_label_offset` — смещение надписи с названием маршрута относительно координат конечной остановки на карте. Массив из двух элементов типа double. Задаёт значения свойств `dx` и `dy` `SVG`-элемента `text`. Элементы массива — числа в диапазоне от `–100000 до 100000`.  
`stop_label_font_size` — размер текста, которым отображаются названия остановок. Целое число в диапазоне `от 0 до 100000`.  
`stop_label_offset` — смещение названия остановки относительно её координат на карте. Массив из двух элементов типа double. Задаёт значения свойств `dx` и `dy` SVG-элемента `text`. Числа в диапазоне `от –100000 до 100000`.  
`underlayer_color` — цвет подложки под названиями остановок и маршрутов.  
`underlayer_width` — толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута `stroke-width` элемента `<text>`. Вещественное число в диапазоне `от 0 до 100000`.
`color_palette` — цветовая палитра. Непустой массив.  
Цвет можно указать:  
- в виде строки, например, `"red"` или `"black"`;  
- в массиве из трёх целых чисел диапазона `[0, 255]`. Они определяют `r`, `g` и `b` компоненты цвета в формате `svg::Rgb`. Цвет `[255, 16, 12]` нужно вывести как `rgb(255, 16, 12)`;  
- в массиве из четырёх элементов: три целых числа в диапазоне от `[0, 255]` и одно вещественное число в диапазоне от `[0.0, 1.0]`. Они задают составляющие `red`, `green`, `blue` и `opacity` цвета формата `svg::Rgba`. Цвет, заданный как `[255, 200, 23, 0.85]`, должен быть выведен как `rgba(255, 200, 23, 0.85)`.

#### Структура словаря routing_settings
```
"routing_settings": {
      "bus_wait_time": 6,
      "bus_velocity": 40
} 
```
`bus_wait_time` — время ожидания автобуса на остановке, в минутах. Считайте, что когда бы человек ни пришёл на остановку и какой бы ни была эта остановка, он будет ждать любой автобус в точности указанное количество минут. Значение — целое число `от 1 до 1000`.  
`bus_velocity` — скорость автобуса, в км/ч. Считайте, что скорость любого автобуса постоянна и в точности равна указанному числу. Время стоянки на остановках не учитывается, время разгона и торможения тоже. Значение — вещественное число `от 1 до 1000`.  
Данная конфигурация задаёт время ожидания, равным 6 минутам, и скорость автобусов, равной 40 километрам в час.

---
### Запросы к базе транспортного справочника

#### Запрос на получение информации об автобусном маршруте:
```
{
  "id": 12345678,
  "type": "Bus",
  "name": "14"
} 
```
Ключ `name` задаёт название маршрута, для которого приложение должно вывести статистическую информацию.  
В ответ на этот запрос выдается в виде словаря:
```
{
  "curvature": 2.18604,
  "request_id": 12345678,
  "route_length": 9300,
  "stop_count": 4,
  "unique_stop_count": 3
} 
```
В словаре содержатся ключи:
`curvature` — число типа double, задающее извилистость маршрута. Извилистость равна отношению длины дорожного расстояния маршрута к длине географического расстояния;  
`request_id` — целое число, равное `id` соответствующего запроса `Bus`;  
`route_length` — целое число, равное длине маршрута в метрах;  
`stop_count` — количество остановок на маршруте;  
`unique_stop_count` — количество уникальных остановок на маршруте.  
На кольцевом маршруте, заданном остановками `A, B, C, A`, количество остановок равно четырём, а количество уникальных остановок равно трём.  
На некольцевом маршруте, заданном остановками `A, B и C`, количество остановок равно пяти `(A, B, C, B, A)`, а уникальных — равно трём.  

#### Запрос на получение информации об автобусной остановке:
```
{
  "id": 12345,
  "type": "Stop",
  "name": "Улица Докучаева"
} 
```
Ключ `name` задаёт название остановки.  
Ответ на запрос:
```
{
  "buses": [
      "14", "22к"
  ],
  "request_id": 12345
} 
```
#### Запрос на получение изображения:
```
{
  "type": "Map",
  "id": 11111
}
```
Ответ на запрос:
```
{
  "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
  "request_id": 11111
} 
```
Ключ `map` — строка с изображением карты в формате `SVG`
![image](https://github.com/kutuzzov/cpp-transport-catalogue/blob/master/route.png)


### Запрос на построение маршрута между двумя остановками
Помимо стандартных свойств `id` и `type`, запрос содержит ещё два:  
`from` — остановка, где нужно начать маршрут.  
`to` — остановка, где нужно закончить маршрут.  
Оба значения — названия существующих в базе остановок. Однако они, возможно, не принадлежат ни одному автобусному маршруту.
```
{
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Universam",
      "id": 4
}
```
Ответ на запрос:
```
{
          "items": [
              {
                  "stop_name": "Biryulyovo Zapadnoye",
                  "time": 6,
                  "type": "Wait"
              },
              {
                  "bus": "297",
                  "span_count": 2,
                  "time": 5.235,
                  "type": "Bus"
              },
              {
                  "stop_name": "Universam",
                  "time": 6,
                  "type": "Wait"
              },
              {
                  "bus": "635",
                  "span_count": 1,
                  "time": 6.975,
                  "type": "Bus"
              }
          ],
          "request_id": 5,
          "total_time": 24.21
      }
 ```
---
