## RESP.app Extension Server

Developers love Redis because it gives freedom to store anything they want in it.
RESP.app shares this ideology by supporting automatic decompression (GZIP, LZ4, ZSTD, BROTLI, Snappy) and deserialization of common formats like MsgPack, PHP Sessions, CBOR and Pickle.

Is your serialization format not mentioned above? Continue reading to find out how to easily view your data in RESP.app.

### What is it?

Starting from version `2022.4` RESP.app comes with a built-in client for Extension Server. Extension Server is a simple REST API defined by
the following [OpenAPI Specification](extension-server.md#openapi-v3-specification). This server allows you to support 
any custom compression or serialization format.

### Build your own Extension Server in minutes

Thanks to [OpenAPI Generator](https://openapi-generator.tech/docs/installation) you can generate boilerplate for your Extension Server in a couple of minutes.

1. [Install OpenAPI Generator](https://openapi-generator.tech/docs/installation)
2. Select [appropriate server generator](https://openapi-generator.tech/docs/generators#server-generators).
3. Download spec file from `https://raw.githubusercontent.com/uglide/RedisDesktopManager/2022/docs/server_spec.yaml`
4. Generate server: <br />
`
openapi-generator generate -i server_spec.yaml -g YOUR_GENERATOR -o my_extension_server
`
5. Open `my_extension_server` in your favorite IDE and start adding your custom formatters to generated server.


**If you are faced with any issues you can [contact support](mailto:support@resp.app) or ask for help in [telegram chat](https://t.me/RedisDesktopManager)**


### Connect to Extension Server in RESP.app

1. Ensure that you are using RESP.app version `2022.4` or above
2. Click on the "Extension Server" button in top right corner of the main window
3. In the Extension Server dialog specify your server URL and basic auth details if any:
<img src="http://resp.app/static/docs/extension-server.png" width="550" />
4. Hit Reload button

### Visualizing data with Extension Server

RESP.app supports following `Content-Type` responses from Extension Server:

- `application/json`
- `image/*` for example `image/svg+xml`

This allows you to perform any required preprocessing and visualize your data:
<img src="http://resp.app/static/docs/extension-server-chart.png" width="550" />



### OpenAPI v3 Specification

**Please submit your proposals to the following spec on [GitHub](https://github.com/uglide/RedisDesktopManager/issues)**

!!swagger server_spec.yaml!!


### Third-party extension servers

You can find some examples on [GitHub](https://github.com/search?q=resp.app+extension+server).
