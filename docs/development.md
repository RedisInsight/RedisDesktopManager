### Build RDM from source
See [instruction](install.md#build-from-source)

### Generate test data
Open RDM console or redis-cli and execute:

```lua
eval "for index = 0,100000 do redis.call('SET', 'test_key' .. index, index) end" 0
eval "for index = 0,100000 do redis.call('SET', 'test_key:' .. math.random(1, 100) .. ':' .. math.random(1,100), index) end" 0
eval "for index = 0,100000 do redis.call('HSET', 'test_large_hash', index, index) end" 0
eval "for index = 0,100000 do redis.call('ZADD', 'test_large_zset', index, index) end" 0
eval "for index = 0,100000 do redis.call('SADD', 'test_large_set', index) end" 0
eval "for index = 0,100000 do redis.call('LPUSH', 'test_large_list', index) end" 0
```

### App profiling

```bash
sudo apt-get install valgrind
sudo add-apt-repository ppa:kubuntu-ppa/backports 
sudo apt-get update
sudo apt-get install massif-visualizer

export LD_LIBRARY_PATH="/usr/share/redis-desktop-manager/lib":$LD_LIBRARY_PATH
valgrind --tool=massif --massif-out-file=rdm.massif /usr/share/redis-desktop-manager/bin/rdm

```

### Debug SSL
```bash
openssl s_client -connect HOST:PORT -cert test_user.crt -key test.key -CAfile test_ca.pem
```

### Remove app settings on OSX
```bash
rm $HOME/Library/Preferences/com.redisdesktop.RedisDesktopManager.plist
killall -u `whoami` cfprefsd
```

### Fix bugs or implement whatever you want :)
