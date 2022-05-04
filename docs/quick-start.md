# **How to start using RESP.app**
***


After you've [installed](install.md) RESP.app, the first thing you need to do in order to get going is to create a connection to your Redis server. On the main window, press the button labelled **Connect to Redis Server**. 

![](http://resp.app/static/docs/rdm_main.png?v=2)

## Connect to a local or public redis-server
On the first tab (Connection Settings), put in general information regarding the connection that you are creating.

* **Name** - the name of new connection (example: my_local_redis)
* **Host** - redis-server host (example: localhost)
* **Port** - redis-server port (example: 6379)
* **Password** - redis-server authentication password (if any) ([http://redis.io/commands/AUTH](http://redis.io/commands/AUTH))
* **Username** - only for redis-servers >= 6.0 with configured [ACL](https://redis.io/topics/acl), for older redis-server leave empty

## Connect to a public redis-server with SSL
If you want to connect to a redis-server instance with SSL you need to enable SSL on the second tab and provide a public key in PEM format. 
Instructions for certain cloud services are below:

<img src="http://resp.app/static/docs/rdm_ssl.png?v=2" />

### AWS ElastiCache
AWS ElastiCache is not accessible outside of your VPC. In order to connect to your ElastiCache remotely, you need to use one of the following options:

*  Setup VPN connection **[Recommended]**
[https://docs.aws.amazon.com/AmazonElastiCache/latest/red-ug/accessing-elasticache.html#access-from-outside-aws](https://docs.aws.amazon.com/AmazonElastiCache/latest/red-ug/accessing-elasticache.html#access-from-outside-aws)
*  Setup SSH proxying host and connect through SSH tunnel. **[Slow network performance. Not recommended]**
*  Setup NAT instance for exposing your AWS ElastiCache to the Internet **[Firewall rules should be configured very carefully. Not recommended.]**

#### How to connect to AWS ElastiCache with In-Transit Encryption
##### VPN / NAT
Enable SSL/TLS checkbox and connect to your AWS ElastiCache with In-Transit Encryption.

##### SSH tunnel
Click on "Enable TLS-over-SSH" checkbox in the the SSH connection settings and connect to your AWS ElastiCache with In-Transit Encryption.

### Microsoft Azure Redis Cache <br /> <img src="https://docs.microsoft.com/en-us/azure/azure-cache-for-redis/media/index/redis-cache.svg" width="100" />

1. Create a connection with all requested information.
 <br /> <img src="http://resp.app/static/docs/rdm_ssl_azure.png?v=2" />
2. Make sure that the "Use SSL Protocol" checkbox is enabled.
3. Your Azure Redis connection is ready to use.

### Redis Labs <br /> <img src="https://upload.wikimedia.org/wikipedia/commons/7/75/Redis_Labs_Logo.png" height="75" />
To connect to a Redis Labs instance with SSL/TLS encryption, follow the steps below:

1. Make sure that SSL is enabled for your Redis instance in the Redis Labs dashboard.
2. Download and unzip `garantia_credentials.zip` from the Redis Labs dashboard.
3. Select `garantia_user.crt` in the "Public key" field.
4. Select `garantia_user_private.key` in the "Private key" field.
5. Select `garantia_ca.pem` in the "Authority" field.

### Digital Ocean Managed Redis <br /> <img src="https://upload.wikimedia.org/wikipedia/commons/f/ff/DigitalOcean_logo.svg" width="100">
Digital Ocean connection settings is a bit confusing. To connect to a Digital Ocean Managed Redis you need to follow steps bellow:

1. Copy host, port and password information to RESP.app
2. **Leave Username field in RESP.app empty** (Important!)
3. Enable SSL/TLS checkbox

Or using Quick Connect tab for new connections:
1. Copy connection string (starts with "rediss://") from connection details to RESP.app
2. Click "Import" and "Test Connection"

### Heroku Redis <br /> <img src="https://brand.heroku.com/static/media/heroku-logo-stroke.aa0b53be.svg" width="80">

1. Get Redis connection string from terminal with command 
```
heroku config -a YOUR-APP-NAME | grep REDIS
```
or get it from Heroku website.

Example output:
```
rediss://user:password@host:port
```
2. Enter connection settings in RESP.app Connection dialog:
- If URL starts with `rediss` enable SSL/TLS checkbox and **uncheck** "Enable strict mode" checkbox
- Copy `user` to "Username" field
- Copy `password` to "Password" field
- Copy `host` and `port` to "Address" field in RESP.app

## Connect to private redis-server via SSH tunnel
### Basic SSH tunneling
SSH tab is supposed to allow you to use a SSH tunnel. It's useful if your redis-server is not publicly accessible.
To use a SSH tunnel select checkbox "SSH Tunnel". There are different security options; you can use a plain password or OpenSSH private key. 

>!!! note "for Windows users:" 
    Your private key must be in .pem format.

<img height="350" src="http://resp.app/static/docs/resp_ssh.png?v=1" />

### SSH Agent
Starting from version 2022.3 RESP.app supports SSH Agents. This allows using password managers like [1Password](https://developer.1password.com/docs/ssh/agent)
to securely store your SSH keys with 2FA.

>!!! note "for Windows users:" 
    On Windows RESP.app supports only [Microsoft OpenSSH](https://docs.microsoft.com/en-us/windows-server/administration/openssh/openssh_overview) that's why "Custom SSH Agent Path" option is not available.  

##### How to connect to 1Password SSH-Agent from DMG version of RESP.app
It's possible to set default SSH Agent for all connections in RESP.app by overriding environment variable `SSH_AUTH_SOCK`.
If you need to use custom ssh agent only for specific connections follow steps above:

1. Create symlink to agent.sock
```
mkdir -p ~/.1password && ln -s ~/Library/Group\ Containers/2BUA8C4S2C.com.1password/t/agent.sock ~/.1password/agent.sock
```
2. In RESP.app check "Use SSH Agent" checkbox and click on the "Select File" button next to "Custom SSH Agent Path" field
3. Press `⌘ + Shift + .` to show hidden files and folders in the dialog
4. Select file `~/.1password/agent.sock`
5. Save connection settings

##### How to connect to SSH-Agent from AppStore version of RESP.app

Due to AppStore sandboxing RESP.app cannot access default or custom SSH Agents defined by `SSH_AUTH_SOCK` variable.
To overcome this limitation you need to create proxy unix socket inside RESP.app sandbox container:

1. Install socat with homebrew
```
brew install socat
```
2. Create proxy unix-socket with socat:
```
socat UNIX-LISTEN:$HOME/Library/Containers/com.redisdesktop.rdm/Data/agent.sock UNIX-CONNECT:"$HOME/Library/Group Containers/2BUA8C4S2C.com.1password/t/agent.sock"
```


### Advanced SSH tunneling
If you need advanced SSH tunneling you should setup a SSH tunnel manually and connect via localhost:
```
ssh SSH_HOST -L 7000:localhost:6379
```

## Connect to a UNIX socket

RESP.app [doesn't support UNIX sockets](https://github.com/uglide/RedisDesktopManager/issues/1751) directly, but you can use redirecting of the local socket to the UNIX domain socket, for instance with [socat](https://sourceforge.net/projects/socat):

```
socat -v tcp-l:6379,reuseaddr,fork unix:/tmp/redis.sock
```

## Advanced connection settings
The **Advanced settings** tab allows you to customise the namespace separator and other advanced settings.
<img src="http://resp.app/static/docs/rdm_advanced_settings.png?v=3" />

## Next steps
Now you can test a connection or create a connection right away. 

Congratulations, you've successfully connected to your Redis Server. You should see something similar to what we show below.
![](http://resp.app/static/docs/rdm_main2.png?v=2)


Click on the connection and expand keys. By clicking the right button, you can see console menu and manage your connection from there. 
