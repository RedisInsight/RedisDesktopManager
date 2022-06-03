# Bulk operations
***

RESP.app simplifies your Redis daily routines with bulk operations. To access bulk operations connect to Redis
server and click on a target database like db0:

<img src="http://resp.app/static/docs/bulk_operations.png?v=1" width="350" />

## Supported bulk operations

<img src="http://resp.app/static/docs/bulk_operations_list.png?v=1" width="350" />

### Flush database

It's a useful operation if you need to invalidate cache in a couple clicks instead of firing `FLUSHDB` command.
> !!! warning "Be careful"
    Do not use it on Production servers. You can safeguard your Production Redis server by using [a restricted user with limited permissions](https://redis.io/docs/manual/security/acl/).

### Delete keys with filter

If you need to remove some specific keys or a ["namespace"](lg-keyspaces.md#use-namespaced-keys) from your Redis server this bulk operation can come in handy.
It allows you to specify a glob style pattern to define which keys should be removed.

![](http://resp.app/static/docs/bulk_delete_keys.png?v=1)

### Set TTL for multiple keys

As you know, Redis is an in-memory database. You should be careful and set appropriate TTL for all keys otherwise Redis can 
crash or stop responding after consuming all available memory. If you realized that some keys have wrong TTL values or don't have TTL at all you can fix it using RESP.app:

![](http://resp.app/static/docs/bulk_ttl.png?v=1)


### Copy keys from one Redis server to another

Sometimes you need to copy some keys from a Production Redis server to local one for debugging or vice-versa.
You can achieve that by writing custom script, however it's much easier to just make a couple of clicks in RESP.app to copy keys:

> !!! warning "Limitations"
    Currently RESP.app supports only copying data between redis-servers with the same RDB version. 
    Usually it means that major versions of both Redis servers should be the same.  

![](http://resp.app/static/docs/bulk_copy_keys.png?v=1)


### Import keys directly from RDB files

Usually, production Redis servers have [AOF or RDB back-ups or persistent files](https://redis.io/docs/manual/persistence/).
While AOF is basically a file with all commands that should be played again to reconstruct original dataset, RDB files don't have such flexibility.
Therefore, RESP.app provides a convenient way to easily import subset of data for debugging and testing directly from RDB file instead of creating additional load to your Production system.

![](http://resp.app/static/docs/bulk_import_rdb.png?v=1)


#### Is your use case not covered in RESP.app? [Contact us](mailto:support@resp.app), and we will do our best to solve it!  
