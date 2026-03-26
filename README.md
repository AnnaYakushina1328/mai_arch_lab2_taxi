# mai_arch_lab2_taxi
2 лаба по программной инженерии 16 вариант
# Домашнее задание 02: Разработка REST API сервиса

## Содержание

- [Задание](#задание)
- [Результат](#результат)
- [Вариант 16 — Система заказа такси](#вариант-16--система-заказа-такси)
- [Архитектура](#архитектура)
- [Структура проекта](#структура-проекта)
- [REST API](#rest-api)
  - [POST /users](#post-users)
  - [GET /users](#get-users)
  - [POST /auth/login](#post-authlogin)
  - [POST /drivers](#post-drivers)
  - [POST /rides](#post-rides)
  - [GET /rides](#get-rides)
  - [PATCH /rides/{id}/accept](#patch-ridesidaccept)
  - [PATCH /rides/{id}/complete](#patch-ridesidcomplete)
- [Аутентификация](#аутентификация)
- [Middleware](#middleware)
- [Тестирование](#тестирование)
- [HTTP статус-коды](#http-статус-коды)
- [OpenAPI](#openapi)
- [Хранилище](#хранилище)
- [Запуск](#запуск)
  - [Локальный запуск в userver-контейнере](#локальный-запуск-в-userver-контейнере)
  - [Запуск через Docker Compose](#запуск-через-docker-compose)
- [Ограничения текущей реализации](#ограничения-текущей-реализации)
- [Вывод](#вывод)

## Задание

Цель работы: получить практические навыки разработки REST API сервиса с использованием принципов REST, обработки HTTP-запросов, реализации аутентификации, middleware и документирования API.

В рамках лабораторной работы требовалось:

1. Спроектировать REST API для выбранной предметной области.
2. Реализовать REST API сервис на выбранном языке и фреймворке.
3. Реализовать минимум 5 API endpoint-ов из выбранного варианта.
4. Реализовать простую аутентификацию.
5. Защитить минимум 2 endpoint с помощью аутентификации.
6. Добавить middleware для проверки аутентификации.
7. Создать OpenAPI/Swagger спецификацию.
8. Подготовить простые тесты успешных и ошибочных сценариев.
9. Подготовить Docker-совместимый запуск сервиса.

## Результат

Реализован REST API сервис системы заказа такси на **C++20** с использованием **Yandex userver**.

Сервис поддерживает:

- создание нового пользователя
- поиск пользователя по логину
- поиск пользователя по маске имени
- логин пользователя и получение токена
- регистрацию водителя
- создание заказа поездки
- получение активных поездок
- получение истории поездок пользователя
- принятие поездки водителем
- завершение поездки

Также реализованы:

- Bearer token аутентификация
- middleware для проверки токена
- OpenAPI спецификация
- smoke-тесты и error-тесты
- запуск через Docker и Docker Compose

## Вариант 16 — Система заказа такси

Приложение содержит следующие основные сущности:

- **Пользователь**
- **Водитель**
- **Поездка**

Реализованы следующие операции:

- создание нового пользователя
- поиск пользователя по логину
- поиск пользователя по маске имени и фамилии
- регистрация водителя
- создание заказа поездки
- получение активных заказов
- принятие заказа водителем
- получение истории поездок пользователя
- завершение поездки

## Архитектура

Сервис реализован как **один backend-сервис `taxi_service`** на **Yandex userver**.

Основные части решения:

- модели предметной области: `User`, `Driver`, `Ride`
- in-memory хранилище: `TaxiStorage`
- userver-компонент для доступа к хранилищу: `TaxiStorageComponent`
- HTTP handler-ы для работы с пользователями, водителями и поездками
- middleware для проверки Bearer token
- OpenAPI спецификация
- bash-тесты для позитивных и негативных сценариев
- Dockerfile и docker-compose для запуска

## Структура проекта

Ниже показана структура проекта в виде дерева каталогов:

```text
taxi_service/
├── CMakeLists.txt
├── CMakePresets.json
├── Makefile
├── Dockerfile
├── docker-compose.yaml
├── README.md
├── openapi.yaml
├── .dockerignore
├── configs/
│   ├── config_vars.yaml
│   ├── config_vars.testing.yaml
│   └── static_config.yaml
├── src/
│   ├── main.cpp
│   ├── models/
│   │   ├── user.hpp
│   │   ├── driver.hpp
│   │   └── ride.hpp
│   ├── storage/
│   │   ├── taxi_storage.hpp
│   │   ├── taxi_storage.cpp
│   │   ├── taxi_storage_component.hpp
│   │   └── taxi_storage_component.cpp
│   ├── handlers/
│   │   ├── users_create.hpp
│   │   ├── users_create.cpp
│   │   ├── users_get.hpp
│   │   ├── users_get.cpp
│   │   ├── auth_login.hpp
│   │   ├── auth_login.cpp
│   │   ├── drivers_create.hpp
│   │   ├── drivers_create.cpp
│   │   ├── rides_create.hpp
│   │   ├── rides_create.cpp
│   │   ├── rides_get.hpp
│   │   ├── rides_get.cpp
│   │   ├── rides_accept.hpp
│   │   ├── rides_accept.cpp
│   │   ├── rides_complete.hpp
│   │   └── rides_complete.cpp
│   └── middlewares/
│       ├── auth_middleware.hpp
│       └── auth_middleware.cpp
└── tests/
    ├── smoke_tests.sh
    ├── error_tests.sh
    ├── conftest.py
    ├── pytest.ini
    └── test_basic.py
```

## REST API

### POST /users

Создание нового пользователя.

**Request**

```json
{
  "login": "passenger1",
  "password": "secret123",
  "full_name": "Ivan Ivanov"
}
```

**Response**

```json
{
  "id": 1,
  "login": "passenger1",
  "full_name": "Ivan Ivanov"
}
```

### GET /users

Поиск пользователя.

**Поиск по логину**

```http
GET /users?login=passenger1
```

**Response**

```json
{
  "id": 1,
  "login": "passenger1",
  "full_name": "Ivan Ivanov"
}
```

**Поиск по маске имени**

```http
GET /users?name_mask=Ivan
```

**Response**

```json
[
  {
    "id": 1,
    "login": "passenger1",
    "full_name": "Ivan Ivanov"
  }
]
```

### POST /auth/login

Логин пользователя и получение токена.

**Request**

```json
{
  "login": "passenger1",
  "password": "secret123"
}
```

**Response**

```json
{
  "token": "token-1-1"
}
```

### POST /drivers

Регистрация водителя.

Требует авторизацию.

**Headers**

```http
Authorization: Bearer token-1-1
```

**Request**

```json
{
  "user_id": 1,
  "car_model": "Toyota Camry",
  "car_number": "A123BC777",
  "license_number": "77AB123456"
}
```

**Response**

```json
{
  "id": 1,
  "user_id": 1,
  "car_model": "Toyota Camry",
  "car_number": "A123BC777",
  "license_number": "77AB123456",
  "status": "online"
}
```

### POST /rides

Создание заказа поездки.

Требует авторизацию.

**Headers**

```http
Authorization: Bearer token-1-1
```

**Request**

```json
{
  "passenger_id": 1,
  "pickup_address": "Moscow City",
  "destination_address": "Sheremetyevo"
}
```

**Response**

```json
{
  "id": 1,
  "passenger_id": 1,
  "driver_id": 0,
  "pickup_address": "Moscow City",
  "destination_address": "Sheremetyevo",
  "status": "searching"
}
```

### GET /rides

Получение поездок.

**Активные поездки**

```http
GET /rides?status=active
```

**Response**

```json
[
  {
    "id": 1,
    "passenger_id": 1,
    "driver_id": 0,
    "pickup_address": "Moscow City",
    "destination_address": "Sheremetyevo",
    "status": "searching"
  }
]
```

**История поездок пользователя**

```http
GET /rides?user_id=1
```

**Response**

```json
[
  {
    "id": 1,
    "passenger_id": 1,
    "driver_id": 1,
    "pickup_address": "Moscow City",
    "destination_address": "Sheremetyevo",
    "status": "completed"
  }
]
```

### PATCH /rides/{id}/accept

Принятие поездки водителем.

Требует авторизацию.

**Headers**

```http
Authorization: Bearer token-1-1
```

**Request**

```json
{
  "driver_id": 1
}
```

**Пример**

```http
PATCH /rides/1/accept
```

**Response**

```json
{
  "id": 1,
  "passenger_id": 1,
  "driver_id": 1,
  "pickup_address": "Moscow City",
  "destination_address": "Sheremetyevo",
  "status": "accepted"
}
```

### PATCH /rides/{id}/complete

Завершение поездки.

Требует авторизацию.

**Headers**

```http
Authorization: Bearer token-1-1
```

**Пример**

```http
PATCH /rides/1/complete
```

**Response**

```json
{
  "id": 1,
  "passenger_id": 1,
  "driver_id": 1,
  "pickup_address": "Moscow City",
  "destination_address": "Sheremetyevo",
  "status": "completed"
}
```

## Аутентификация

В проекте реализована простая token-based аутентификация.

Пользователь получает токен через endpoint:

```http
POST /auth/login
```

Далее токен передаётся в заголовке:

```http
Authorization: Bearer <token>
```

Защищены следующие endpoint-ы:

- `POST /drivers`
- `POST /rides`
- `GET /rides`
- `PATCH /rides/{id}/accept`
- `PATCH /rides/{id}/complete`

## Middleware

Для проверки токена реализован отдельный middleware.

Middleware:

- перехватывает входящие HTTP-запросы
- пропускает публичные endpoint-ы:
  - `/ping`
  - `/users`
  - `/auth/login`
  - `/tests/*`
- для защищённых endpoint-ов проверяет наличие заголовка `Authorization`
- проверяет формат `Bearer <token>`
- валидирует токен через `TaxiStorage`
- в случае ошибки возвращает `401 Unauthorized`

Таким образом, проверка аутентификации вынесена из логики handler-ов в отдельный слой.

## Тестирование

Для тестирования реализованы два bash-скрипта:

- `tests/smoke_tests.sh` — успешные сценарии
- `tests/error_tests.sh` — ошибочные сценарии

### Smoke tests

Проверяются:

- создание пользователя
- логин
- регистрация водителя
- создание поездки
- получение активных поездок
- принятие поездки
- завершение поездки
- получение истории поездок пользователя

**Запуск**

```bash
bash -x ./tests/smoke_tests.sh
```

### Error tests

Проверяются:

- запрос `GET /users` без query-параметров
- логин с неверным паролем
- доступ к защищённому endpoint без токена
- доступ к защищённому endpoint с неверным токеном

**Запуск**

```bash
bash -x ./tests/error_tests.sh
```

## HTTP статус-коды

В сервисе используются следующие основные HTTP статус-коды:

| Код | Статус | Описание | Примеры |
|---|---|---|---|
| `200 OK` | Успешное выполнение | Запрос выполнен успешно | успешный `GET`, логин, принятие и завершение поездки |
| `201 Created` | Ресурс создан | Новый ресурс успешно создан | создание пользователя, регистрация водителя, создание поездки |
| `400 Bad Request` | Некорректный запрос | Переданы невалидные данные или отсутствуют обязательные параметры | пустое тело запроса, отсутствуют query-параметры, неверный формат данных |
| `401 Unauthorized` | Неавторизован | Отсутствует токен, токен неверен или переданы неверные учётные данные | доступ к защищённому endpoint без токена, неверный токен, неверный логин или пароль |
| `404 Not Found` | Ресурс не найден | Запрашиваемый ресурс отсутствует | пользователь, водитель или поездка с указанным идентификатором не найдены |
| `409 Conflict` | Конфликт состояния | Операция не может быть выполнена из-за текущего состояния данных | пользователь уже существует, водитель уже зарегистрирован, поездка уже принята или уже завершена |

## OpenAPI

Для проекта подготовлена спецификация:

```text
openapi.yaml
```

В ней описаны:

- все endpoint-ы
- параметры запросов
- request/response схемы
- ошибки
- Bearer token security scheme

## Хранилище

Хранилище реализовано как in-memory storage.

Используются контейнеры в памяти процесса:

- пользователи
- водители
- поездки
- токены

Плюсы такого решения:

- простота реализации
- быстрый запуск
- отсутствие внешней базы данных

Минус:

- данные не сохраняются между перезапусками сервиса или контейнера

## Запуск

### Локальный запуск в userver-контейнере

Из каталога проекта:

```bash
docker run --rm -it \
  --user "$(id -u):$(id -g)" \
  --security-opt seccomp=unconfined \
  -p 8080:8080 \
  -v "$PWD":/work \
  -w /work \
  --entrypoint bash \
  ghcr.io/userver-framework/ubuntu-22.04-userver:latest
```

Внутри контейнера:

```bash
cd taxi_service
export HOME=/tmp
export CCACHE_DIR=/tmp/.ccache
mkdir -p "$CCACHE_DIR"
make build-debug
./build-debug/taxi_service -c configs/static_config.yaml
```

### Запуск через Docker Compose

Из каталога `mai_arch_lab2_taxi`:

```bash
docker compose up --build
```

или

```bash
git clone https://github.com/AnnaYakushina1328/mai_arch_lab2_taxi.git
cd mai_arch_lab2_taxi
docker compose up --build
```

**Проверка**

```bash
curl -i "http://127.0.0.1:8080/ping"
```

## Ограничения текущей реализации

- используется in-memory storage вместо PostgreSQL или SQLite
- данные не переживают перезапуск процесса
- токены не имеют срока жизни
- роли пользователей не разделяются
- нет геолокации, расчёта стоимости и рейтингов
- сервис реализован как один backend, а не как распределённая микросервисная система

## Вывод

В ходе лабораторной работы был разработан REST API сервис системы заказа такси на C++ с использованием Yandex userver.

В проекте реализованы:

- операции над основными сущностями предметной области
- поиск ресурсов через query-параметры
- простая token-based аутентификация
- middleware для проверки авторизации
- OpenAPI документация
- smoke- и error-тесты
- Docker-совместимый запуск
