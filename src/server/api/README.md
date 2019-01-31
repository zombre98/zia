# dems API Zia
##### The goal of this Api is to provide a example on How to add modules to your Zia project
###### Produced by Armand Megrot, Bilel Fourati, Anatole Juge and Thomas Burgaud

Our goal for this API is to simplify the way to add modules in your ZIA<br/>
For this we have our best tried to minimize the constraints that we impose<br/>

For example to add your modules you just have to make : 
```cpp
extern "C" {

    void registersHook(api::StageManager &manager) {
        manager.requests().hookToEnd("Example", [](api::Context &ctx) {
            std::cout << "I'm an example module" << std::endl;
            return api::CodeStatus::OK;
        });
    }

};
```

## AModulesManager

The implementation of AModulesManager **should** be instanciated **only one time** in your project.<br/>
It will let you load a directory of modules or a single module through the function `loadModules`on `loadOneModule`.<br/>
AModulesManager bring you an implementation of StatesManager through `getStageManager`

Each module need to `extern` a function called `registersHook`
You should give them your StageManager. She will use the StageManager to hook* his functions where he want.<br/>
For that you will use a `registersHook` to hook your functions.

For example, in pseudo-code, if you call your **StageManager** `manager` : <br/>
`manager.stage.moment("moduleName", module)`

\* **A hook** is a function that will be called each time that a Stage it's triggered in one of three differents Stage 

## StageManager

StageManager will manage differents Stage in your Server.
We implement four Stage :<br/>

| Stage      | Description | 
| :----:       |    :----:     |
| **connection** | It should provide hooks* for when the clients connect on the server.     |
| **request** | When a server receive a request, the provided hooks of this Stage should be called.       |
| **chunks**  | When a data it received by chunk (see rfc2616) it should called the provided hooks of this stage        |
| **disconnection**  | It should provide hooks for when the clients disconnect on the server.        |


## Stage

| Moment      | Description | 
| :----:       |    :----:     |
| **first** | It's called before middle when the event occurs (Example : SSL module)     |
| **middle** | It's called before last during the event (Example : PHP module)       |
| **last**  | It's called in the last moment of the event (Example : Encryption SSL module) |

Example : 

When a request is received :

She will trigger the "request" stage. So **you** will have to make the 
call to the Stage and to the differents "Moments" provided:

Request triggered =>
Call the "request" Stage => 
call the functions hooked by the modules =>
Firsts hooks Called =>
Middles Hooks Called =>
Lasts Hooks Called.

For example if you want to Hook a module Function to the beginning of the stage `request` you will have to do :

`manager.requests().hookToFirst("moduleName", std::function<CodeStatus(Context &)>)`

As you see the function take a Context that will explain later and return Status code defined in enum : 
 ```cpp
 enum class CodeStatus {
 	OK, // If the module accept the call
 	DECLINED, // If he decline
 	HTTP_ERROR // If there is an error
 };
 ```
 
 ## Context
 
 When a Stage is triggered he will call to the different hook and will give them a Context.
 
 The **Context** is defined as the following :
 
 ```cpp
 struct Request {
 	std::string method;
 	std::string path;
 	std::string httpVersion;
 };
 
 struct Response {
 	std::string httpVersion;
 	std::string statusCode;
 	std::string message;
 };
 
struct HTTPMessage {
	std::variant<Request, Response> firstLine;
	std::unique_ptr<IHeading> header;
	std::string body;
};

struct Context {
 	header::HTTPMessage request;
 	header::HTTPMessage response;
 	int socketFd;
 };
 ```

#### Struct Context

```cpp
struct Context {
 	header::HTTPMessage request;
 	header::HTTPMessage response;
 	int socketFd;
 };
```

From the structure `Context`, the field `request` will contain the original request of the client.<br/>
The field `response` should be constructed by the differents modules.<br/>
We give you the field `socketFd` if you want to read or write data depending on the modules. 
 
#### Struct HTTPMessage

```cpp
struct HTTPMessage {
	std::variant<Request, Response> firstLine;
	std::unique_ptr<IHeading> header;
	std::string body;
};
```

The `firstLine` field contains a Request or Response depending on which HTTPMessage you are : <br/>
* In `request` the type of firstLine will be a `struct Request`
* In `response` the type of firstLine will be a `struct Response`

To use a `std::variant` here is an example :
```cpp
dems::Context context{{dems::header::Request{"GET", "/path/file", "HTTP/1.1"}, std::make_unique<dems::header::Heading>(), ""},
                     {dems::header::Response{"HTTP/1.1", "200", "OK"},std::make_unique<dems::header::Heading>(), ""}, 0};


std::cout << std::get<dems::header::Request>(context.request.variant).path << std::endl;
```
On the fist line we show you how to create a `dems::Context`<br/>
On the last line we take the path from the variant Request by using `std::get<T>`, `T` is the type you want (in this case is either `Request` or `Response`).
