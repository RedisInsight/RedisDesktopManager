# **How to start using RDM**
***


After you [install](install.md)  RDM the first you need to do in order to start using it is create Connection to your Redis server. On the main screen press button **Connect to Redis Server**. 

![](http://redisdesktop.com/docs/rdm_main.png)

## Connect to local or public redis-server
On the first tab Connection Settings put general info regarding connection that you are creating.  

* **Name** - name of new connection (example: my_local_redis)
* **Host** - redis-server host (example: localhost)
* **Port** - redis-server port (example: 6379)
* **Auth** - redis-server authentication password ([http://redis.io/commands/AUTH](http://redis.io/commands/AUTH))

## Connect to public redis-server with SSL
If you want to connect to redis-server with SSL you need to enable SSL on the second tab and provide a public key in PEM format. Bellow you can find instructions for particular cloud services.

<img src="http://redisdesktop.com/docs/rdm_ssl.png" height="300" />

### Windows Azure Redis Cache
1. Copy Primary key from Azure Portal to AUTH field on first tab
2. Also Copy SSL port  to Port field on first tab
3. Retrieve current Azure public key and save in azure.pem file:

```openssl x509 -in <(openssl s_client -connect YOUR_SERVER:YOUR_SSL_PORT -prexit 2>/dev/null)```

*NOTE for Windows users:* The easiest way to run this command in Windows - MSYS2

4 Provide path to azure.pem file on SSL tab in Public key field

### Redislabs
**TBD**

## Connect to private redis-server via SSH tunnel
### Basic SSH tunneling
SSH tab is supposed to allow you using SSH tunnel. It's useful if your redis-server is not publicly accessible via a network.
To use SSH tunnel select checkbox "Use SSH Tunnel". There are different security options: you can use a plain password or openssh private key. 

> **NOTE for Windows users:** Your private key is must be in .pem format.

<img src="http://redisdesktop.com/docs/rdm_ssh.png" height="300" />

### Advanced SSH tunneling
If you need advanced SSH tunneling you should setup SSH tunnel manually and connect via localhost:
```
ssh SSH_HOST -L 7000:localhost:6379
```

## Connect to UNIX socket

Redis Desktop Manager [doesn't support UNIX sockets](https://github.com/uglide/RedisDesktopManager/issues/1751) directly, but you can use redirecting of local socket to the unix domain socket, for instance with [socat](http://sourceforge.net/projects/socat):

```
socat -v tcp-l:6379,reuseaddr,fork unix:/tmp/redis.sock
```

## Advanced connection settings
Use the next tab Advanced settings in order to setup Namespace separator or other settings. 


<img src="http://redisdesktop.com/docs/rdm_advanced_settings.png" height="300" />

## Next steps
Now you can test connection or create connection right away. 

Congratulations, now you connected to your Redis Server. You should see something similar to what we showed on our screen.
![](http://redisdesktop.com/docs/rdm_main2.png)


Click on the connection and expand keys. By clicking the right button, you can see console menu and manage your connection from there. 