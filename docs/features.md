## Search in connection tree
RDM supports key filtering. To filter keys you need to open database context menu and select "Filter keys"

![](https://redisdesktop.com/static/docs/rdm_tree_search1.png)

After that you will be able to enter filter pattern:

![](https://redisdesktop.com/static/docs/rdm_tree_search2.png)

Press OK button to apply a filter.


## Native value formatters

Native value formatters are scripts or executables developed in different programming languages which allow
 to display in readable form the key values of various formats inside the RDM value editor. You can easily implement any formatter for your own purposes as well. To see more information including the full list of available formatters and implementation instructions please follow this [link](https://github.com/RedisDesktop/rdm-native-value-formatters).


### Installation

To install the formatter create `formatters/` directory inside of `.rdm/` settings directory which is by default located inside of your home directory or by the path you specify as `--settings-dir` option when you run RDM. Then copy formatter's directory into `.rdm/formatters/`. Or just clone the repository into `.rdm/` like shown below.

```shell
cd ~/.rdm/
git clone https://github.com/RedisDesktop/rdm-native-value-formatters.git formatters
```


#### Snappy note

If you are going to use Snappy formatter make sure to install `libsnappy-dev` by running:

```shell
sudo apt-get install libsnappy-dev
```


### Install Python requirements

For now most of the formatters are implemented in Python 3 and require additional packages for proper work. So before using formatters make sure you have `python3` and `pip3` on your system and then run `install-python-deps.sh` to install all they need. To achieve this simply run the following commands.

```shell
cd formatters/
chmod u+x install-python-deps.sh
./install-python-deps.sh
```


### How to verify installation

To verify that the formatter is installed go to Settings and check the table titled "Custom Value View Formatters" for desired formatter presence.
![](settings_formatters.png)


### How to use

Click on the key to open the value editor and choose the appropriate formatter from the "View as:" drop-down menu.
