## Limitations of App Store version

* AppStore version of RDM doesn't support [Native Formatters](native-formatters.md)
* Current version doesn't support connection to open-source redis-servers with self-signed TLS certificates. [More info](https://github.com/uglide/RedisDesktopManager/issues/4944)
  <br> Workaround: Use DMG version of RDM or disable mutual TLS in redis-server settings (`REDIS_TLS_AUTH_CLIENTS: "no"`)
  
  
