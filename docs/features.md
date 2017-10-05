## Search in connection tree
RDM supports key filtering. To filter keys you need to open database context menu and select "Filter keys"

![](https://redisdesktop.com/static/docs/rdm_tree_search1.png)

After that you will be able to enter filter pattern:

![](https://redisdesktop.com/static/docs/rdm_tree_search2.png)

Press OK button to apply a filter.


## Native value formatters

Formatters allow you to view the key values of various formats.

Supported formats:
* [Pickle](https://docs.python.org/3/library/pickle.html)
* [MSGPack](http://msgpack.org/)
* [CBOR](http://cbor.io/)
* Compressed (supported algorithms: gzip, lzma, lz4, snappy)

Formatters are available on [GitHub](https://github.com/RedisDesktop/rdm-native-value-formatters).


### Installation

To install the formatter you need to copy its directory into ~/.rdm/formatters/ which is by default located inside your home directory.


### Ubuntu/Fedora/OS X

Some of the formatters have dependencies which are specified in their requirements files, so before using the formatter run the script install-python-deps.sh


### How to verify installation

To verify that the formatter is installed go to settings and check out the table titled Custom Value View Formatters and if the formatters you need are not there verify that they are located the Formatters path.


### How to use

Click on the key to open the value editor and choose the appropriate formatter from the "View as:" drop-down menu.