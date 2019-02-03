HTTP server in cpp

## Module API

```objectivec
namespace zia {
    struct IModule {
      bool preConfig();
      Directive configDirectives[];
      void registerHooks(ModuleManager &);
    }
};
```
* `preConfig` -- Called before configuration files are parsed. Provides the ability for modules to load default configuration values and initialize the module (e.g. seed PRNGs, etc...)
* `configDirectives`  -- Tells the server which directives your module accepts (cf. )
* `registerHooks` -- Allows the module to register callback function request process stages (cf. 'Hooks' section)
