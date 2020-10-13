# Working with large keyspaces

## Use specific `SCAN` filter to reduce loaded amount of keys


By default RDM uses `*` (wildcard glob-style pattern) in  `SCAN` command to load all keys from the selected database. It’s simple and user-friendly for cases when you have only a couple of thousands keys. But for production redis-servers with millions of keys it leads to a huge amount of time needed to load keys in RDM. 

However, while working with production redis-servers that contain millions of keys users have to wait for a long time till all keys are loaded. 

Consider using more specific  filters for `SCAN` in order to speed up keys loading and reduce memory footprint 

1. Right click on database and click on Filter button <br> <img width="250" src="https://user-images.githubusercontent.com/1655867/91542521-aa18c300-e926-11ea-8f09-4a0322d0f9ee.png">
2. Enter glob-style pattern and press apply button <br> <img width="250" src="https://user-images.githubusercontent.com/1655867/91542549-b4d35800-e926-11ea-9920-ca0ad8701c56.png">

More details about `SCAN` filter syntax you can find in Redis documentation https://redis.io/commands/scan#the-match-option


Default `SCAN` filter can be changed in connection settings on “Advanced Settings” tab:
<br /><img width="500" src="https://user-images.githubusercontent.com/1655867/91543353-1eebfd00-e927-11ea-81ed-90bcc25c41f0.png">


## Use namespaced keys

Colon sign `:` is a commonly used convention when naming Redis keys. For example you can use following schema to store information about users:

`user:1000`

Following this schema allows you to simplify removal of obsolete keys and performing other operations with keys in Redis.

Using namespaced keys is also important for loading huge keyspaces in RDM. It renders namespaces on demand (since 2020.2+) and this approach allows to visualise millions of keys with small memory footprint.

<img width="250" src="https://user-images.githubusercontent.com/1655867/91547979-5c538900-e92d-11ea-8afa-10cc1634343f.png">

Default namespace separator can be changed in connection settings on “Advanced Settings” tab.

More tips about Redis keys naming you can find in this tutorial https://redis.io/topics/data-types-intro#redis-keys

