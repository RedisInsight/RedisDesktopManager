## Search in connection tree
RDM supports key filtering. To filter keys you need to open database context menu and select "Filter keys"

![](https://redisdesktop.com/static/docs/rdm_tree_search1.png)

After that you will be able to enter filter pattern:

![](https://redisdesktop.com/static/docs/rdm_tree_search2.png)

Press OK button to apply a filter.


## Native value formatters

Formatters allow to display the key values of various formats in the value editor in readable form.

Supported formats:
* [Pickle](https://docs.python.org/3/library/pickle.html)
* [MSGPack](http://msgpack.org/)
* [CBOR](http://cbor.io/)
* Compressed (supported algorithms: gzip, lzma, lz4, snappy)

Formatters are available on [GitHub](https://github.com/RedisDesktop/rdm-native-value-formatters).
If these are not enough you can implement your own formatters by following [instructions](https://github.com/RedisDesktop/rdm-native-value-formatters/blob/master/README.md).


### Installation

To install the formatter create `formatters/` directory inside of `.rdm/` settings directory which is by default located inside of your home directory or by the path you specify as `--settings-dir` option when you run RDM. Then just copy formatter's directory into `.rdm/formatters/`.


### Ubuntu/Fedora/OS X

Some of the formatters have `requirements.txt` file which contains packages required for proper work. So before using the formatter copy `install-python-deps.sh` into `.rdm/formatters/` and run the script to get all of these packages installed.


### How to verify installation

To verify that the formatter is installed go to Settings and check the table titled "Custom Value View Formatters" for desired formatter presence.


### How to use

Click on the key to open the value editor and choose the appropriate formatter from the "View as:" drop-down menu.