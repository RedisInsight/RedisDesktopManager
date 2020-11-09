# **How to start using RDM**
***


After you've [installed](install.md) RDM, the first thing you need to do in order to get going is to create a connection to your Redis server. On the main window, press the button labelled **Connect to Redis Server**. 

![](http://rdm.dev/static/docs/rdm_main.png?v=2)

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

<img src="http://rdm.dev/static/docs/rdm_ssl.png?v=2" />

### AWS ElastiCache
AWS ElastiCache is not accessible outside of your VPC. In order to connect to your ElastiCache remotely, you need to use one of the following options:

*  Setup VPN connection **[Recommended]**
[https://docs.aws.amazon.com/AmazonElastiCache/latest/red-ug/accessing-elasticache.html#access-from-outside-aws](https://docs.aws.amazon.com/AmazonElastiCache/latest/red-ug/accessing-elasticache.html#access-from-outside-aws)
*  Setup SSH proxying host and connect through SSH tunnel. **[Slow network performance. Not recommended]**
*  Setup NAT instance for exposing your AWS ElastiCache to the Internet **[Firewall rules should be configured very carefully. Not recommended.]**

#### How to connect to AWS ElastiCache with In-Transit Encryption
##### VPN / NAT
Enable SSL/TLS checkox and connect to your AWS ElastiCache with In-Transit Encryption.

##### SSH tunnel
Click on "Enable TLS-over-SSH" checkbox in the the SSH connection settings and connect to your AWS ElastiCache with In-Transit Encryption.

### Windows Azure Redis Cache <br /> <img src="https://docs.microsoft.com/en-us/azure/azure-cache-for-redis/media/index/redis-cache.svg" width="100" />

1. Create a connection with all requested information.
 <br /> <img src="http://rdm.dev/static/docs/rdm_ssl_azure.png?v=2" />
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

1. Copy host, port and password information to RDM
2. **Leave Username field in RDM empty** (Important!)
3. Enable SSL/TLS checkox


## Connect to private redis-server via SSH tunnel
### Basic SSH tunneling
SSH tab is supposed to allow you to use a SSH tunnel. It's useful if your redis-server is not publicly accessible.
To use a SSH tunnel select checkbox "Use SSH Tunnel". There are different security options; you can use a plain password or OpenSSH private key. 

>!!! note "for Windows users:" 
    Your private key must be in .pem format.

<img src="http://rdm.dev/static/docs/rdm_ssh.png?v=2" />

### Advanced SSH tunneling
If you need advanced SSH tunneling you should setup a SSH tunnel manually and connect via localhost:
```
ssh SSH_HOST -L 7000:localhost:6379
```

## Connect to a UNIX socket

RDM [doesn't support UNIX sockets](https://github.com/uglide/RedisDesktopManager/issues/1751) directly, but you can use redirecting of the local socket to the UNIX domain socket, for instance with [socat](https://sourceforge.net/projects/socat):

```
socat -v tcp-l:6379,reuseaddr,fork unix:/tmp/redis.sock
```

## Advanced connection settings
The **Advanced settings** tab allows you to customise the namespace separator and other advanced settings.
<img src="http://rdm.dev/static/docs/rdm_advanced_settings.png?v=3" />

## Next steps
Now you can test a connection or create a connection right away. 

Congratulations, you've successfully connected to your Redis Server. You should see something similar to what we show above.
![](http://rdm.dev/static/docs/rdm_main2.png?v=2)


Click on the connection and expand keys. By clicking the right button, you can see console menu and manage your connection from there. 
