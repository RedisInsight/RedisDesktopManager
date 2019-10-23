# **How to start using RDM**
***


After you've [installed](install.md) Redis Desktop Manager, the first thing you need to do in order to get going is to create a connection to your Redis server. On the main window, press the button labelled **Connect to Redis Server**. 

![](http://redisdesktop.com/static/docs/rdm_main.png?v=2)

## Connect to a local or public redis-server
On the first tab (Connection Settings), put in general information regarding the connection that you are creating.

* **Name** - the name of new connection (example: my_local_redis)
* **Host** - redis-server host (example: localhost)
* **Port** - redis-server port (example: 6379)
* **Auth** - redis-server authentication password (if any) ([http://redis.io/commands/AUTH](http://redis.io/commands/AUTH))

## Connect to a public redis-server with SSL
If you want to connect to a redis-server instance with SSL you need to enable SSL on the second tab and provide a public key in PEM format. 
Instructions for certain cloud services are below:

<img src="http://redisdesktop.com/static/docs/rdm_ssl.png?v=2" />


### Windows Azure Redis Cache <br /> <img src="https://docs.microsoft.com/en-us/azure/azure-cache-for-redis/media/index/redis-cache.svg" width="100" />

1. Create a connection with all requested information.
 <br /> <img src="http://redisdesktop.com/static/docs/rdm_ssl_azure.png?v=2" />
2. Make sure that the "Use SSL Protocol" checkbox is enabled.
3. Your Azure Redis connection is ready to use.

### Redis Labs <br /> <img src="https://upload.wikimedia.org/wikipedia/commons/7/75/Redis_Labs_Logo.png" height="75" />
To connect to a Redis Labs instance with SSL/TLS encryption, follow the steps below:

1. Make sure that SSL is enabled for your Redis instance in the Redis Labs dashboard.
2. Download and unzip `garantia_credentials.zip` from the Redis Labs dashboard.
3. Select `garantia_user.crt` in the "Public key" field.
4. Select `garantia_user_private.key` in the "Private key" field.
5. Select `garantia_ca.pem` in the "Authority" field.


## Connect to private redis-server via SSH tunnel
### Basic SSH tunneling
SSH tab is supposed to allow you to use a SSH tunnel. It's useful if your redis-server is not publicly accessible.
To use a SSH tunnel select checkbox "Use SSH Tunnel". There are different security options; you can use a plain password or OpenSSH private key. 

>!!! note "for Windows users:" 
    Your private key must be in .pem format.

<img src="http://redisdesktop.com/static/docs/rdm_ssh.png?v=2" />

### Advanced SSH tunneling
If you need advanced SSH tunneling you should setup a SSH tunnel manually and connect via localhost:
```
ssh SSH_HOST -L 7000:localhost:6379
```

### How to connect to Redis ElastiCache through EC2 
Follow instructions from [this blog post](https://userify.com/blog/howto-connect-redis-ec2-ssh-tunnel-elasticache/)

### How to connect to Redis ElastiCache with In-Transit Encryption through EC2
#### Using RedisDesktopManager >=0.9.9

Click on "Enable TLS-over-SSH" in the the SSH connection settings and follow instructions from section [How to connect to Redis ElastiCache through EC2](#how-to-connect-to-redis-elasticache-through-ec2)


#### Using RedisDesktopManager <0.9.9
Follow instructions from [this documentation section to setup `stunnel` on an EC2 instance](https://docs.aws.amazon.com/AmazonElastiCache/latest/red-ug/in-transit-encryption.html#connect-tls)

After that [connect to your Redis ElastiCache through EC2](#how-to-connect-to-redis-elasticache-through-ec2).

## Connect to a UNIX socket

Redis Desktop Manager [doesn't support UNIX sockets](https://github.com/uglide/RedisDesktopManager/issues/1751) directly, but you can use redirecting of the local socket to the UNIX domain socket, for instance with [socat](https://sourceforge.net/projects/socat):

```
socat -v tcp-l:6379,reuseaddr,fork unix:/tmp/redis.sock
```

## Advanced connection settings
The **Advanced settings** tab allows you to customise the namespace separator and other advanced settings.
<img src="http://redisdesktop.com/static/docs/rdm_advanced_settings.png?v=2" />

## Next steps
Now you can test a connection or create a connection right away. 

Congratulations, you've successfully connected to your Redis Server. You should see something similar to what we show above.
![](http://redisdesktop.com/static/docs/rdm_main2.png?v=2)


Click on the connection and expand keys. By clicking the right button, you can see console menu and manage your connection from there. 
