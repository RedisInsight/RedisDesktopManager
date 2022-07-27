<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>QObject</name>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="360"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="397"/>
        <source>Cannot connect to cluster node %1:%2</source>
        <translation>无法连接集群节点 %1:%2</translation>
    </message>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="408"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="435"/>
        <source>Cannot flush db (%1): %2</source>
        <translation>无法刷新库 (%1): %2</translation>
    </message>
</context>
<context>
    <name>RESP</name>
    <message>
        <location filename="../../app/app.cpp" line="82"/>
        <source>Settings directory is not writable</source>
        <translation>设置保存文件夹没有写入权限</translation>
    </message>
    <message>
        <location filename="../../app/app.cpp" line="84"/>
        <source>RESP.app can&apos;t save connections file to settings directory. Please change file permissions or restart RESP.app as administrator.</source>
        <translation>RESP.app 无法将连接文件保存到设置目录。 请更改文件权限或以管理员身份重新启动 RESP.app。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="81"/>
        <source>Cannot rename key %1: %2</source>
        <translation>无法重命名键 %1: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="125"/>
        <source>Cannot persist key &apos;%1&apos;. &lt;br&gt; Key does not exist or does not have an assigned TTL value</source>
        <translation>无法持久化键 &apos;%1&apos;，&lt;br&gt; 键不存在或没有设置TTL时长</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="160"/>
        <source>Cannot parse scan response</source>
        <translation>无法解析扫描结果</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="328"/>
        <source>Server returned unexpected response: </source>
        <translation>服务器返回了意外结果：</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="106"/>
        <source>Cannot set TTL for key %1</source>
        <translation>无法给键 %1 设置 TTL</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="274"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="285"/>
        <source>Cannot load rows for key %1: %2</source>
        <translation>无法加载键内容 %1: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="42"/>
        <location filename="../../app/models/key-models/hashkey.cpp" line="75"/>
        <location filename="../../app/models/key-models/listkey.cpp" line="14"/>
        <location filename="../../app/models/key-models/listkey.cpp" line="41"/>
        <location filename="../../app/models/key-models/setkey.cpp" line="12"/>
        <location filename="../../app/models/key-models/setkey.cpp" line="33"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="44"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="77"/>
        <location filename="../../app/models/key-models/stream.cpp" line="48"/>
        <location filename="../../app/models/key-models/stream.cpp" line="59"/>
        <source>Invalid row</source>
        <translation>无效行</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="113"/>
        <source>Value with the same key already exists</source>
        <translation>同名键值已经存在</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="184"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="340"/>
        <location filename="../../app/models/treeoperations.cpp" line="151"/>
        <location filename="../../modules/console/consolemodel.cpp" line="84"/>
        <source>Connection error: </source>
        <translation>连接错误：</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="136"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="136"/>
        <source>Data was loaded from server partially.</source>
        <translation>部分数据已经从服务器加载。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="26"/>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="38"/>
        <source>Cannot load key %1, connection error occurred: %2</source>
        <translation>无法加载键 %1，连接发生错误：%2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="49"/>
        <source>Cannot load key %1 because it doesn&apos;t exist in database. Please reload connection tree and try again.</source>
        <translation>无法加载键 %1，数据库中不存在该键，请重载连接树后重试。</translation>
    </message>
    <message>
        <source>Cannot load TTL for key %1, connection error occurred: %2</source>
        <translation type="vanished">无法加载键 %1 的 TTL 值，连接发生错误: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="78"/>
        <source>Cannot retrieve type of the key: </source>
        <translation>无法重设键类型：</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="122"/>
        <source>Cannot open file with key value</source>
        <translation>无法用键值打开文件</translation>
    </message>
    <message>
        <source>Unsupported Redis Data type %1</source>
        <translation type="vanished">数据格式不支持 %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="129"/>
        <source>Cannot connect to server &apos;%1&apos;. Check log for details.</source>
        <translation>无法连接到服务器 &apos;%1&apos;，详情请查看日志。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="139"/>
        <source>Open Source version of RESP.app &lt;b&gt;doesn&apos;t support SSH tunneling&lt;/b&gt;.&lt;br /&gt;&lt;br /&gt; To get fully-featured application, please buy subscription on &lt;a href=&apos;https://resp.app/subscriptions&apos;&gt;resp.app&lt;/a&gt;. &lt;br/&gt;&lt;br /&gt;Every single subscription gives us funds to continue the development process and provide support to our users. &lt;br /&gt;If you have any questions please feel free to contact us at &lt;a href=&apos;mailto:support@resp.app&apos;&gt;support@resp.app&lt;/a&gt; or join &lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;.</source>
        <translation>RESP.app 的开源版本&lt;b&gt;不支持 SSH 隧道&lt;/b&gt;。&lt;br /&gt;&lt;br /&gt; 要获得功能齐全的应用程序，请在 &lt;a href=&apos;https://resp 上购买订阅 .app/subscriptions&apos;&gt;resp.app&lt;/a&gt;。 &lt;br/&gt;&lt;br /&gt;每一次订阅都为我们提供了资金来继续开发过程并为我们的用户提供支持。 &lt;br /&gt;如果您有任何问题，请随时通过 &lt;a href=&apos;mailto:support@resp.app&apos;&gt;support@resp.app&lt;/a&gt; 与我们联系或加入 &lt;a href=&apos;https:// t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;。 </translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="229"/>
        <source>Cannot load keys: %1</source>
        <translation>无法加载键：%1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="336"/>
        <source>Delete key error: %1</source>
        <translation>删除键失败: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="477"/>
        <source>Cannot determine amount of used memory by key: %1</source>
        <translation>无法调用该键占用的内存: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="416"/>
        <source>Cannot flush database: </source>
        <translation>清空库错误:</translation>
    </message>
    <message>
        <location filename="../../modules/common/tabmodel.cpp" line="43"/>
        <source>Invalid Connection. Check connection settings.</source>
        <translation>无效连接，请检查连接设置。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="251"/>
        <source>Live update was disabled due to exceeded keys limit. Please specify filter more carefully or change limit in settings.</source>
        <translation>由于超出加载键数量限制，实时更新功能已经关闭。请设置更精确的筛查条件或更改加载限制设定。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="304"/>
        <source>Key was added. Do you want to reload keys in selected database?</source>
        <translation>键已经添加。需要重新加载该数据库的键名吗？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="312"/>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="143"/>
        <source>Key was added</source>
        <translation>键已经插入</translation>
    </message>
    <message>
        <source>Another operation is currently in progress</source>
        <translation type="vanished">另一个操作正在进行中</translation>
    </message>
    <message>
        <source>Please wait until another operation will be finished.</source>
        <translation type="vanished">请耐心等待另一个操作完成。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="327"/>
        <source>Do you really want to remove all keys from this database?</source>
        <translation>确定要删除该数据库里面所有的键吗？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="73"/>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="75"/>
        <source>Cannot load databases:

</source>
        <translation>无法加载数据库：

</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="250"/>
        <source>Live update was disabled</source>
        <translation>实时更新已关闭</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="183"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="190"/>
        <source>Rename key</source>
        <translation>重命名键</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="199"/>
        <source>New name:</source>
        <translation>新名称：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="21"/>
        <source>Total pages: </source>
        <translation>总页数: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="45"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="222"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="358"/>
        <source>Size: </source>
        <translation>大小：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="228"/>
        <source>TTL:</source>
        <translation>TTL：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="233"/>
        <source>Set key TTL</source>
        <translation>设置键的 TTL</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="242"/>
        <source>New TTL:</source>
        <translation>新的 TTL：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="298"/>
        <source>Delete</source>
        <translation>删除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="23"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="303"/>
        <source>Delete key</source>
        <translation>删除键</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="38"/>
        <source>Changes are not saved</source>
        <translation>更改未保存</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="39"/>
        <source>Do you want to close key tab without saving changes?</source>
        <translation>不保存更改关闭标签页吗？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="260"/>
        <source>Persist key</source>
        <translation>持久化键</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="304"/>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="153"/>
        <source>Do you really want to delete this key?</source>
        <translation>确定要删除该键？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="140"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="318"/>
        <source>Reload Value</source>
        <translation>重载键值</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="31"/>
        <source>Add Row</source>
        <translation>插入行</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="30"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="526"/>
        <source>Add Element to HLL</source>
        <translation>添加元素到HLL</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="68"/>
        <source>Add</source>
        <translation>添加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="101"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="122"/>
        <source>Delete row</source>
        <translation>删除行</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="107"/>
        <source>The row is the last one in the key. After removing it key will be deleted.</source>
        <translation>此行数据是该键最后一行数据。删除此行数据，该键将会被删除。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="109"/>
        <source>Do you really want to remove this row?</source>
        <translation>确定要删除该行数据吗？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="164"/>
        <source>Search on page...</source>
        <translation>页面搜索中...</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="191"/>
        <source>Full Search</source>
        <translation>全文搜索</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="191"/>
        <source>Value and Console tabs related to this connection will be closed. Do you want to continue?</source>
        <translation>所有与该连接相关的键值对话框和命令操作对话框都将被关闭，确定要继续吗？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="204"/>
        <source>Do you really want to delete connection?</source>
        <translation>确定要删除连接？</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="13"/>
        <source>Connected to cluster.
</source>
        <translation>已连接到集群。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="16"/>
        <source>Connected.
</source>
        <translation>已连接。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="60"/>
        <source>Switch to %1 mode. Close console tab to stop listen for messages.</source>
        <translation>切换到推送/订阅模式，关闭标签页来停止接收信息。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="69"/>
        <source>Subscribe error: %1</source>
        <translation>订阅错误：%1</translation>
    </message>
    <message>
        <location filename="../../modules/server-actions/serverstatsmodel.cpp" line="36"/>
        <source>Server %0</source>
        <translation>服务器 %0</translation>
    </message>
    <message>
        <source>Can&apos;t find formatter with name: %1</source>
        <translation type="vanished">找不到格式化配置名：%1</translation>
    </message>
    <message>
        <location filename="../../modules/extension-server/dataformattermanager.cpp" line="109"/>
        <location filename="../../modules/extension-server/dataformattermanager.cpp" line="147"/>
        <source>Can&apos;t find formatter: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../modules/extension-server/dataformattermanager.cpp" line="115"/>
        <source>Invalid callback</source>
        <translation>无效回调</translation>
    </message>
    <message>
        <location filename="../../modules/extension-server/dataformattermanager.cpp" line="204"/>
        <source>Can&apos;t load list of available formatters from extension server: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../modules/extension-server/dataformattermanager.cpp" line="260"/>
        <source>Can&apos;t encode value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Cannot decode value using %1 formatter. </source>
        <translation type="vanished">无法使用 %1 格式化配置来解析值。</translation>
    </message>
    <message>
        <source>Cannot validate value using %1 formatter.</source>
        <translation type="vanished">无法使用 %1 格式化配置来效验值。</translation>
    </message>
    <message>
        <source>Cannot encode value using %1 formatter. </source>
        <translation type="vanished">无法使用 %1 编码键值</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="26"/>
        <source>Loading key: %1 from db %2</source>
        <translation>从 db %2 加载 key: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="68"/>
        <source>Cannot open value tab</source>
        <translation>无法打开键值对话框</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="97"/>
        <source>Connection error</source>
        <translation>连接错误</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="115"/>
        <source>Connection error. Can&apos;t open value tab. </source>
        <translation>连接错误，无法打开键值对话框。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="176"/>
        <source>Cannot reload key value: %1</source>
        <translation>无法重载键值: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="228"/>
        <source>Cannot load key value: %1</source>
        <translation>无法加载键值：%1</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="29"/>
        <source>Connect to Redis Server</source>
        <translation>连接到 Redis 服务器</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="117"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="205"/>
        <source>Import</source>
        <translation>导入</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="50"/>
        <location filename="../../qml/AppToolBar.qml" line="69"/>
        <source>Import Connections</source>
        <translation>导入连接</translation>
    </message>
    <message>
        <source>Export</source>
        <translation type="obsolete">导出</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="58"/>
        <location filename="../../qml/AppToolBar.qml" line="74"/>
        <source>Export Connections</source>
        <translation>导出连接</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="100"/>
        <source>Report issue</source>
        <translation>报告错误</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="107"/>
        <source>Documentation</source>
        <translation>文档</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="114"/>
        <source>Join Telegram Chat</source>
        <translation>加入 Telegram 聊天组</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="121"/>
        <source>Follow</source>
        <translation>关注</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="128"/>
        <source>Star on GitHub!</source>
        <translation>给我们的GitHub加个星星吧！</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="136"/>
        <source>Log</source>
        <translation>日志</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="144"/>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="13"/>
        <source>Extension Server</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="154"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="13"/>
        <source>Settings</source>
        <translation>设置</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="10"/>
        <source>New Connection Settings</source>
        <translation>新连接设置</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="144"/>
        <source>How to connect</source>
        <translation>怎么连接</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="151"/>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="48"/>
        <source>Connection Settings</source>
        <translation>连接设置</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="180"/>
        <source>Create connection from Redis URL</source>
        <translation>从Redis URL创建连接</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="233"/>
        <source>Learn more about Redis URL:  </source>
        <translation>认识Redis URL: </translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="240"/>
        <source>Connection guides</source>
        <translation>连接向导</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="260"/>
        <source>Local or Public Redis</source>
        <translation>本地或对外的Redis</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="276"/>
        <source>Redis with SSL/TLS</source>
        <translation>使用SSL/TLS的Redis</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="292"/>
        <source>SSH tunnel</source>
        <translation>SSH通道</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="308"/>
        <source>UNIX socket</source>
        <translation>UNIX套接字</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="395"/>
        <source>Cannot figure out how to connect to your redis-server?</source>
        <translation>不知道怎么连接到您的Redis服务端吗？</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="401"/>
        <source>&lt;a href=&quot;https://docs.resp.app/en/latest/quick-start/&quot;&gt;Read the Docs&lt;/a&gt;, &lt;a href=&quot;mailto:support@resp.app&quot;&gt;Contact Support&lt;/a&gt; or ask for help in our &lt;a href=&quot;https://t.me/RedisDesktopManager&quot;&gt;Telegram Group&lt;/a&gt;</source>
        <translation type="unfinished">&lt;a href=&quot;https://docs.resp.app/en/latest/quick-start/&quot;&gt;查看文档&lt;/a&gt;, &lt;a href=&quot;mailto:support@resp.app&quot;&gt;联系支持&lt;/a&gt; 或者点这里寻求帮助 &lt;a href=&quot;https://t.me/RedisDesktopManager&quot;&gt;Telegram Group&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="413"/>
        <source>Don&apos;t have running Redis?</source>
        <translation>Redis没有启动吗？</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="419"/>
        <source>Spin up hassle-free Redis on Digital Ocean</source>
        <translation>快速使用Digital Ocean上的Redis</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="431"/>
        <source>Skip</source>
        <translation>跳过</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="466"/>
        <source>Name:</source>
        <translation>名字：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="472"/>
        <source>Connection Name</source>
        <translation>连接名</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="478"/>
        <source>Address:</source>
        <translation>地址</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="483"/>
        <source>redis-server host</source>
        <translation>Redis 服务器地址</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="507"/>
        <source>(Optional) redis-server authentication password</source>
        <translation>(可选) Redis 服务器验证密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="526"/>
        <source>Security</source>
        <translation>安全</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="573"/>
        <source>Public Key:</source>
        <translation>公钥：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="579"/>
        <source>(Optional) Public Key in PEM format</source>
        <translation>(可选) PEM 格式公钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="581"/>
        <source>Select public key in PEM format</source>
        <translation>选择 PEM 格式公钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="592"/>
        <source>(Optional) Private Key in PEM format</source>
        <translation>(可选) PEM 格式私钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="594"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="703"/>
        <source>Select private key in PEM format</source>
        <translation>选择 PEM 格式私钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="599"/>
        <source>Authority:</source>
        <translation>授权：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="605"/>
        <source>(Optional) Authority in PEM format</source>
        <translation>(可选) PEM 格式授权</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="607"/>
        <source>Select authority file in PEM format</source>
        <translation>选择 PEM 格式授权文件</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="553"/>
        <source>SSH Tunnel</source>
        <translation>SSH 通道</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="630"/>
        <source>SSH Address:</source>
        <translation>SSH 地址：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="634"/>
        <source>Remote Host with SSH server</source>
        <translation>SSH 远程服务器</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="642"/>
        <source>SSH User:</source>
        <translation>SSH 用户：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="648"/>
        <source>Valid SSH User Name</source>
        <translation>验证 SSH 用户名</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="586"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="683"/>
        <source>Private Key</source>
        <translation>私钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="701"/>
        <source>Path to Private Key in PEM format</source>
        <translation>PEM 格式私钥路径</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="711"/>
        <source>&lt;b&gt;Tip:&lt;/b&gt; Use &lt;code&gt;⌘ + Shift + .&lt;/code&gt; to show hidden files and folders in dialog</source>
        <translation>&lt;b&gt;提示：&lt;/b&gt; 使用 &lt;code&gt;⌘ + Shift + .&lt;/code&gt; 在对话框中显示隐藏文件和文件夹</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="717"/>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="87"/>
        <source>Password</source>
        <translation>密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="737"/>
        <source>SSH User Password</source>
        <translation>SSH 用户密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="759"/>
        <source>Enable TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</source>
        <translation>启用 TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="156"/>
        <source>Advanced Settings</source>
        <translation>高级设置</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="494"/>
        <source>For better network performance please use 127.0.0.1</source>
        <translation>请使用127.0.0.1获得更好的网络连接速度</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="512"/>
        <source>Username:</source>
        <translation>用户名:</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="518"/>
        <source>(Optional) redis-server authentication username (Redis &gt;6.0)</source>
        <translation>可选：服务端认证用户名 (Redis &gt;6.0)</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="540"/>
        <source>SSL / TLS</source>
        <translation>SSL / TLS</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="612"/>
        <source>Enable strict mode:</source>
        <translation>打开严格模式: </translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="657"/>
        <source>Use SSH Agent</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="667"/>
        <source>(Optional) Custom SSH Agent Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="669"/>
        <source>Select SSH Agent</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="678"/>
        <source>Additional configuration is required to enable SSH Agent support</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="734"/>
        <source>Passphrase for provided private key</source>
        <translation>提供的私钥的密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="736"/>
        <source>Password request will be prompt prior to connection</source>
        <translation>将会在连接前询问密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="747"/>
        <source>Ask for password</source>
        <translation>询问密码</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="796"/>
        <source>Keys loading</source>
        <translation>键加载</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="800"/>
        <source>Default filter:</source>
        <translation>默认过滤：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="806"/>
        <source>Pattern which defines loaded keys from redis-server</source>
        <translation>指定加载键名表达式：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="812"/>
        <source>Namespace Separator:</source>
        <translation>命名空间分隔符：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="819"/>
        <source>Separator used for namespace extraction from keys</source>
        <translation>键名中命名空间分隔符</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="825"/>
        <source>Timeouts &amp; Limits</source>
        <translation>设置超时和限制</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="829"/>
        <source>Connection Timeout (sec):</source>
        <translation>连接超时 (秒)：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="842"/>
        <source>Execution Timeout (sec):</source>
        <translation>执行超时 (秒)：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="853"/>
        <source>Databases discovery limit:</source>
        <translation>数据库发现限制：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="867"/>
        <source>Cluster</source>
        <translation>集群</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="871"/>
        <source>Change host on cluster redirects:</source>
        <translation>修改集群重定向：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="881"/>
        <source>Formatters</source>
        <translation>格式化程序</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="885"/>
        <source>Default value formatter:</source>
        <translation>默认值格式化程序：</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="901"/>
        <source>Auto detect (JSON / Plain Text / HEX)</source>
        <translation>自动检测（JSON / 纯文本 / HEX）</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="902"/>
        <source>Last selected</source>
        <translation>最后选择</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="903"/>
        <source>Select formatter ...</source>
        <translation>选择格式化程序...</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="952"/>
        <source>Appearance</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="956"/>
        <source>Icon color:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1021"/>
        <source>Invalid settings detected!</source>
        <translation>检测到无效的设置！</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="992"/>
        <source>Test Connection</source>
        <translation>测试连接</translation>
    </message>
    <message>
        <location filename="../../qml/common/OkDialogOverlay.qml" line="20"/>
        <location filename="../../qml/common/SaveToFileButton.qml" line="111"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1029"/>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="163"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="319"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="903"/>
        <source>OK</source>
        <translation>确定</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="294"/>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="508"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="402"/>
        <location filename="../../qml/common/BetterDialog.qml" line="44"/>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="61"/>
        <location filename="../../qml/connections/AskSecretDialog.qml" line="89"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1057"/>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="175"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="331"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="172"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="89"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="268"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="48"/>
        <source>General</source>
        <translation>通用</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="53"/>
        <source>Application will be restarted to apply these settings.</source>
        <translation>重启软件来启用新的设置。</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="73"/>
        <source>Language</source>
        <translation>语言</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="393"/>
        <source>Application will be restarted to apply this setting.</source>
        <translation>新设置将在软件重启后生效</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="85"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="174"/>
        <source>Font</source>
        <translation>字体</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="97"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="186"/>
        <source>Font Size</source>
        <translation>字体大小</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="110"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="124"/>
        <source>Dark Mode</source>
        <translation>暗色模式</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="200"/>
        <source>Maximum Formatted Value Size</source>
        <translation>最大格式化长度</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="201"/>
        <source>Size in bytes</source>
        <translation>字节长度</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="259"/>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="392"/>
        <location filename="../../qml/settings/GlobalSettings.qml" line="138"/>
        <source>Use system proxy settings</source>
        <translation>使用系统代理设置</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="150"/>
        <source>Use system proxy only for HTTP(S) requests</source>
        <translation>只对HTTP(S)请求使用系统代理设置</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="156"/>
        <source>Value Editor</source>
        <translation>内容编辑器</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="213"/>
        <source>Maximum amount of items per page</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="218"/>
        <source>Connections Tree</source>
        <translation>连接树</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="236"/>
        <source>Show namespaced keys on top</source>
        <translation>在头部展示命名空间键名</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="246"/>
        <source>Reopen namespaces on reload</source>
        <translation>重载时重新打开命名空间</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="247"/>
        <source>(Disable to improve treeview performance)</source>
        <translation>(禁用树状视图提高性能)</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="258"/>
        <source>Show only last part for namespaced keys</source>
        <translation>仅显示命名空间中键名最后一部分内容</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="270"/>
        <source>Limit for SCAN command</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="282"/>
        <source>Maximum amount of rendered child items</source>
        <translation>子项目的最大渲染数量</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="294"/>
        <source>Live update maximum allowed keys</source>
        <translation>实时更新最大允许键数量</translation>
    </message>
    <message>
        <location filename="../../qml/settings/GlobalSettings.qml" line="306"/>
        <source>Live update interval (in seconds)</source>
        <translation>实时更新间隔 (秒)</translation>
    </message>
    <message>
        <source>External Value View Formatters</source>
        <translation type="vanished">外部键值格式化配置</translation>
    </message>
    <message>
        <source>Formatters path: %0</source>
        <translation type="vanished">格式化配置路径：%0</translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="61"/>
        <source>Server Url:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="75"/>
        <source>Basic Auth:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="81"/>
        <source>User</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="99"/>
        <source>Response timeout  (in seconds)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="109"/>
        <source>Available Data Formatters</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="117"/>
        <source>Reload</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="135"/>
        <source>Id</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="141"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../../qml/extension-server/ExtensionServerSettings.qml" line="147"/>
        <source>Read Only</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/WelcomeTab.qml" line="29"/>
        <source>Version</source>
        <translation>版本</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1001"/>
        <source>Quick Start Guide</source>
        <translation>快速入门指南</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="134"/>
        <source>Successful connection to redis-server</source>
        <translation>连接 Redis 服务器成功</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="137"/>
        <source>Can&apos;t connect to redis-server</source>
        <translation>无法连接 Redis 服务器</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="321"/>
        <source>Add Group</source>
        <translation>添加组</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="336"/>
        <source>Regroup connections</source>
        <translation>重组连接</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="358"/>
        <source>Exit Regroup Mode</source>
        <translation>退出重组模式</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="10"/>
        <source>Bulk Operations Manager</source>
        <translation>批量操作管理</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="66"/>
        <source>Invalid RDB path</source>
        <translation>无效的RDB路径</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="66"/>
        <source>Please specify valid path to RDB file</source>
        <translation>请指定有效的RDB文件路径</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="88"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="89"/>
        <source>Delete keys</source>
        <translation>删除键</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="98"/>
        <source>Set TTL</source>
        <translation>设置TTL</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="106"/>
        <source>Copy keys to another database</source>
        <translation>复制键到其他库</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="107"/>
        <source>Copy keys</source>
        <translation>复制键</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="116"/>
        <source>Import data from rdb file</source>
        <translation>从RDB文件导入数据</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="151"/>
        <source>Redis Server:</source>
        <translation>Redis 服务器：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="163"/>
        <source>Database number:</source>
        <translation>数据库编号：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="183"/>
        <source>Path to RDB file:</source>
        <translation>RDB文件路径：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="205"/>
        <source>Select DB in RDB file:</source>
        <translation>从RDB文件选择库：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="225"/>
        <source>Key pattern:</source>
        <translation>键名表达式：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="225"/>
        <source>Import keys that match &lt;b&gt;regex&lt;/b&gt;:</source>
        <translation>导入匹配&lt;b&gt;regex&lt;/b&gt;的键：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="272"/>
        <source>Destination Redis Server:</source>
        <translation>目标 Redis 服务器：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="283"/>
        <source>Destination Redis Server Database Index:</source>
        <translation>目标 Redis 数据库编号：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="320"/>
        <source>Show matched keys</source>
        <translation>显示匹配的键</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="320"/>
        <source>Show Affected keys</source>
        <translation>显示受影响的键</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="343"/>
        <source>Affected keys:</source>
        <translation>受影响的键：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="343"/>
        <source>Matched keys:</source>
        <translation>匹配的键：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="368"/>
        <source>Bulk Operation finished.</source>
        <translation>批量操作完成。</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="373"/>
        <source>Bulk Operation finished with errors</source>
        <translation>批量操作完成，但发生了一些错误。</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="425"/>
        <source>Processed: </source>
        <translation>已处理：</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="427"/>
        <source>Getting list of affected keys...</source>
        <translation>获取受影响的键列表...</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="475"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1097"/>
        <source>Success</source>
        <translation>成功</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="501"/>
        <source>Confirmation</source>
        <translation>确认</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="502"/>
        <source>Do you really want to perform bulk operation?</source>
        <translation>确认要执行批量操作？</translation>
    </message>
    <message>
        <source>Sign in with resp.app account</source>
        <translation type="obsolete">使用 resp.app 账号登陆</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="231"/>
        <source>Renew your subscription</source>
        <translation>更新订阅</translation>
    </message>
    <message>
        <source>Your trial has ended.</source>
        <translation type="obsolete">试用版已结束</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="234"/>
        <source>You have no active subscription</source>
        <translation>您没有可用订阅</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="237"/>
        <source>No internet connection</source>
        <translation>无网络</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="240"/>
        <source>Your trial has ended</source>
        <translation>您的试用已结束</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="252"/>
        <source>To use this version you need to renew your subscription.</source>
        <translation>要继续使用该版本，需要更新你的订阅。</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="257"/>
        <source>If you’re behind a proxy please enable </source>
        <translation>如果您处于代理之中，请启用代理</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="261"/>
        <source> option before sign-in.</source>
        <translation> 登陆前选项</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="11"/>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="317"/>
        <source>Sign in with RESP.app account</source>
        <translation>使用 RESP.app 帐户登录</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="255"/>
        <source>Please make sure that RESP.app is not blocked by a firewall and you have an internet connection.</source>
        <translation>请确保 RESP.app 未被防火墙阻止并且您有互联网连接。</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="264"/>
        <source>Please purchase a subscription to continue using RESP.app.</source>
        <translation>请购买订阅以继续使用 RESP.app。</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="268"/>
        <source>If you have any questions please contact support </source>
        <translation>遇到任何问题，请联系支持。</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="279"/>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="71"/>
        <source>Renew Subscription</source>
        <translation>更新订阅</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="280"/>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="32"/>
        <source>Buy Subscription</source>
        <translation>购买订阅</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="293"/>
        <source>Try Again</source>
        <translation>重试</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="331"/>
        <source>Email:</source>
        <translation>邮箱：</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="347"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="501"/>
        <source>Password:</source>
        <translation>密码：</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="370"/>
        <location filename="../../qml/common/PasswordInput.qml" line="29"/>
        <source>Show password</source>
        <translation>显示密码</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="376"/>
        <source>Forgot password?</source>
        <translation>忘记密码？</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="404"/>
        <source>Sign In</source>
        <translation>登录</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="409"/>
        <source>Please enter email &amp; password to sign in.</source>
        <translation>请输入邮箱和密码来登录。</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="422"/>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="444"/>
        <source>Offline Activation</source>
        <translation>离线激活</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="465"/>
        <source>Paste Activation code here</source>
        <translation>在这里粘贴激活码</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="481"/>
        <source>Where can I find my activation code?</source>
        <translation>我在哪里能找到我的激活码？</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="490"/>
        <source>Activate</source>
        <translation>激活</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SignInDialog.qml" line="495"/>
        <source>Please enter valid activation code.</source>
        <translation>请输入正确的激活码。</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/TreeItemDelegate.qml" line="220"/>
        <source> (Removed)</source>
        <translation> （删除）</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="77"/>
        <source>Open Keys Filter</source>
        <translation>打开键过滤器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="81"/>
        <source>Reload Keys in Database</source>
        <translation>重载该数据库的键</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="85"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="30"/>
        <source>Add New Key</source>
        <translation>添加新键</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="91"/>
        <source>Disable Live Update</source>
        <translation>关闭实时更新</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="91"/>
        <source>Enable Live Update</source>
        <translation>打开实时更新</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="95"/>
        <source>Open Console</source>
        <translation>打开控制台</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="98"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="32"/>
        <source>Analyze Used Memory</source>
        <translation>分析内存占用</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="100"/>
        <source>Bulk Operations</source>
        <translation>批量操作</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="121"/>
        <source>Flush Database</source>
        <translation>清空数据库</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="124"/>
        <source>Delete keys with filter</source>
        <translation>使用过滤器删除键</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="97"/>
        <location filename="../../qml/connections-tree/menu/database.qml" line="127"/>
        <source>Set TTL for multiple keys</source>
        <translation>设置多个键的TTL</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="130"/>
        <source>Copy keys from this database to another</source>
        <translation>从本库复制键到其他库</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="133"/>
        <source>Import keys from RDB file</source>
        <translation>从RDB文件导入键</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="136"/>
        <source>Back</source>
        <translation>返回</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="22"/>
        <source>Copy Key Name</source>
        <translation>复制键值</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="29"/>
        <source>Reload Namespace</source>
        <translation>重载命名空间</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="31"/>
        <source>Copy Namespace Pattern</source>
        <translation>复制命名空间模式</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="33"/>
        <source>Delete Namespace</source>
        <translation>删除命名空间</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="71"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="24"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="14"/>
        <source>Disconnect</source>
        <translation>断开连接</translation>
    </message>
    <message>
        <source>Server Info</source>
        <translation type="vanished">服务器信息</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="20"/>
        <source>Reload Server</source>
        <translation>重载服务器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="24"/>
        <source>Unload All Data</source>
        <translation>卸载所有数据</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="28"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="10"/>
        <source>Edit Connection Settings</source>
        <translation>编辑连接设置</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="32"/>
        <source>Duplicate Connection</source>
        <translation>复制连接</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="36"/>
        <source>Delete Connection</source>
        <translation>删除连接</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="21"/>
        <source>Connecting...</source>
        <translation>连接中...</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="193"/>
        <source>Clear</source>
        <translation>清除</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="234"/>
        <source>Arguments</source>
        <translation>参数</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="240"/>
        <source>Description</source>
        <translation>描述</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="246"/>
        <source>Available since</source>
        <translation>可用自</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="297"/>
        <source>Close</source>
        <translation>关闭</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="108"/>
        <source>View Server Info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="130"/>
        <source>Redis Version</source>
        <translation>Redis 版本</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="169"/>
        <source>Used memory</source>
        <translation>已使用的内存</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="182"/>
        <source>Cmd Processed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="203"/>
        <source>Monitor Commands</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="242"/>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="319"/>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="105"/>
        <source>Clients</source>
        <translation>客户端</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="377"/>
        <source>Server Actions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Commands Processed</source>
        <translation type="vanished">已执行的命令</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="143"/>
        <source>Uptime</source>
        <translation>运行时间</translation>
    </message>
    <message>
        <source>Total Keys</source>
        <translation type="vanished">键总量</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="156"/>
        <source>Hit Ratio</source>
        <translation>命中率</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="262"/>
        <source>Server Stats</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="281"/>
        <source>Console</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="321"/>
        <source> day(s)</source>
        <translation> 天</translation>
    </message>
    <message>
        <source>Info</source>
        <translation type="vanished">信息</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="53"/>
        <source>Commands Per Second</source>
        <translation>每秒执行命令数</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="67"/>
        <source>Ops/s</source>
        <translation>每秒查询率</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="91"/>
        <source>Connected Clients</source>
        <translation>已连接的客户端数量</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="129"/>
        <source>Memory Usage</source>
        <translation>内存占用</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="141"/>
        <source>Mb</source>
        <translation>Mb</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="170"/>
        <source>Network Input</source>
        <translation>网络输入</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="182"/>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="218"/>
        <source>Kb/s</source>
        <translation>Kb/s</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="206"/>
        <source>Network Output</source>
        <translation>网络输出</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="242"/>
        <source>Total Error Replies</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerCharts.qml" line="256"/>
        <source>Error Replies</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Keys</source>
        <translation type="vanished">键数量</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="36"/>
        <location filename="../../qml/server-actions/ServerConfig.qml" line="28"/>
        <location filename="../../qml/server-actions/ServerSlowlog.qml" line="39"/>
        <source>Auto Refresh</source>
        <translation>自动刷新</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerConfig.qml" line="66"/>
        <source>Property</source>
        <translation>属性</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerConfig.qml" line="72"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="22"/>
        <source>Value</source>
        <translation>键值</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerPubSub.qml" line="68"/>
        <source>Subscribe in Console</source>
        <translation>控制台订阅</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="221"/>
        <source>Slowlog</source>
        <translation>慢查询日志</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerActionTabs.qml" line="299"/>
        <source>Pub/Sub Channels</source>
        <translation>推送/订阅 通道</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerPubSub.qml" line="38"/>
        <source>Enable</source>
        <translation>启用</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerPubSub.qml" line="57"/>
        <source>Channel Name</source>
        <translation>通道名</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerSlowlog.qml" line="54"/>
        <source>Command</source>
        <translation>指令</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerSlowlog.qml" line="71"/>
        <source>Processed at</source>
        <translation>处理于</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerSlowlog.qml" line="86"/>
        <source>Execution Time (μs)</source>
        <translation>执行时长 (μs)</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="51"/>
        <source>Client Address</source>
        <translation>客户端地址</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="57"/>
        <source>Age (sec)</source>
        <translation>时长 (sec)</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="63"/>
        <source>Idle</source>
        <translation>空闲</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="69"/>
        <source>Flags</source>
        <translation>标记</translation>
    </message>
    <message>
        <location filename="../../qml/server-actions/ServerClients.qml" line="75"/>
        <source>Current Database</source>
        <translation>当前库</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="10"/>
        <source>Add New Key to </source>
        <translation>添加新键到 </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="56"/>
        <location filename="../../qml/value-editor/editors/HashItemEditor.qml" line="17"/>
        <source>Key:</source>
        <translation>键名：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="66"/>
        <source>Type:</source>
        <translation>类型：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="107"/>
        <source>Or Import Value from the file</source>
        <translation>或从文件中导入值</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="113"/>
        <source>(Optional) Any file</source>
        <translation>(可选)任意文件</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="115"/>
        <source>Select file with value</source>
        <translation>使用值选择文件</translation>
    </message>
    <message>
        <location filename="../../qml/common/BetterDialog.qml" line="39"/>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="46"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="127"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="254"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="616"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="10"/>
        <source>Edit Connections Group</source>
        <translation>编辑连接组</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="10"/>
        <source>Add New Connections Group</source>
        <translation>添加连接组</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="29"/>
        <source>Group Name:</source>
        <translation>组名:</translation>
    </message>
    <message>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="1091"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="183"/>
        <location filename="../../qml/value-editor/editors/formatters/ValueFormatters.qml" line="251"/>
        <source>Error</source>
        <translation>错误</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="12"/>
        <source>Page</source>
        <translation>页</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="68"/>
        <source>Enter valid value</source>
        <translation>请输入有效的值</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="286"/>
        <source>Formatting error</source>
        <translation>格式化错误</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="291"/>
        <source>Unknown formatter error (Empty response)</source>
        <translation>未知的格式化错误（无响应）</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="363"/>
        <source>[Binary]</source>
        <translation>[二进制]</translation>
    </message>
    <message>
        <source> [Compressed: </source>
        <translation type="obsolete">[压缩的：</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="544"/>
        <source>Copy to Clipboard</source>
        <translation>复制到剪切板</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="600"/>
        <source>Exit </source>
        <translation>退出</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="601"/>
        <source>Full Screen Mode</source>
        <translation>全屏模式</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="617"/>
        <source>Save Changes</source>
        <translation>保存更改</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="719"/>
        <source>Search string</source>
        <translation>搜索字符串</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="733"/>
        <source>Find Next</source>
        <translation>查找下一条</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="733"/>
        <source>Find</source>
        <translation>查找</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="764"/>
        <source>Regex</source>
        <translation>正则表达式</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="750"/>
        <source>Cannot find more results</source>
        <translation>找不到更多的结果</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="400"/>
        <source>Try to decompress:</source>
        <translation>尝试解压:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="401"/>
        <source>Decompressed:</source>
        <translation>解压:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="491"/>
        <source>Cannot decompress value using </source>
        <translation>无法解压以</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="751"/>
        <source>Cannot find any results</source>
        <translation>找不到任何结果</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="899"/>
        <source>Binary value is too large to display</source>
        <translation>二进制内容太长无法展示</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="366"/>
        <source>View as:</source>
        <translation>查看</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="502"/>
        <source>Large value (&gt;150kB). Formatters are not available.</source>
        <translation>键值内容过大（&gt;150kB），格式化配置无效。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="18"/>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="30"/>
        <source>Score</source>
        <translation>分数</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="194"/>
        <source>Path to dump.rdb file</source>
        <translation>导出为 dump.rdb 文件的路径</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="196"/>
        <source>Select dump.rdb</source>
        <translation>选择 dump.rdb</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="18"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="61"/>
        <source>Value (represented as JSON object)</source>
        <translation>值 (表示为 JSON 对象)</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/listkey.cpp" line="127"/>
        <source>The row has been changed on server.Reload and try again.</source>
        <translation>服务端该行内容已经改变，请重新加载。</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/bulkoperationsmanager.cpp" line="131"/>
        <source>Failed to perform actions on %1 keys. </source>
        <translation>对键 %1 执行操作时失败。</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="12"/>
        <source>Cannot copy key </source>
        <translation>无法复制键 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="123"/>
        <source>Source connection error</source>
        <translation>源连接错误</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="135"/>
        <source>Target connection error</source>
        <translation>目标连接错误</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/deleteoperation.cpp" line="11"/>
        <source>Cannot remove key </source>
        <translation>无法清除键 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="17"/>
        <source>Cannot execute command </source>
        <translation>无法执行命令 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="26"/>
        <source>Invalid regexp for keys filter.</source>
        <translation>键筛选表达式无效</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="39"/>
        <source>Cannot get the list of affected keys</source>
        <translation>无法获取受到影响的键列表</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/ttloperation.cpp" line="11"/>
        <source>Cannot set TTL for key </source>
        <translation>无法给该键设置TTL</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/abstractnamespaceitem.cpp" line="381"/>
        <source>Your redis-server doesn&apos;t support &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; commands.</source>
        <translation>你的Redis服务端不支持 &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; 指令</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="138"/>
        <source>Key was added. Do you want to reload keys in selected namespace?</source>
        <translation>键已经添加。需要重新加载选中的命名空间中的键吗？</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="147"/>
        <source>Network is not accessible. Please ensure that you have internet access and try again.</source>
        <translation>网络无效，请确保已经连上互联网，然后重试。</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="190"/>
        <source>Invalid login or password</source>
        <translation>无效的账号或密码</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="200"/>
        <source>Too many requests from your IP</source>
        <translation>过多请求来源你的IP</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="206"/>
        <source>Unknown error. Status code %1</source>
        <translation>未知错误，状态码 %1</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="321"/>
        <location filename="../../modules/api/api_client.cpp" line="734"/>
        <source>Cannot parse server reply</source>
        <translation>无法解析服务器响应</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="341"/>
        <source>Cannot validate token</source>
        <translation>无法效验令牌</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="347"/>
        <source>Cannot login - %1. &lt;br/&gt; Please try again or contact  &lt;a href=&apos;mailto:support@resp.app&apos;&gt;support@resp.app&lt;/a&gt;</source>
        <translation>无法登录 - %1。&lt;br/&gt; 请重试。或直接联系 &lt;a href=&apos;mailto:support@resp.app&apos;&gt;support@resp.app&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="718"/>
        <source>Expired activation code</source>
        <translation>激活码已过期</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="731"/>
        <source>Invalid activation code</source>
        <translation>无效的激活码</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="588"/>
        <location filename="../../modules/api/api_client.cpp" line="614"/>
        <source>Cannot save the update. Disk is full or download folder is not writable.</source>
        <translation>无法保存更新文件，磁盘已满或下载目录无法写入。</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="666"/>
        <source>Download was canceled</source>
        <translation>下载已取消</translation>
    </message>
    <message>
        <location filename="../../modules/api/api_client.cpp" line="673"/>
        <source>Network error</source>
        <translation>网络错误</translation>
    </message>
    <message>
        <location filename="../../qml/common/FilePathInput.qml" line="27"/>
        <source>Select File</source>
        <translation>选择文件</translation>
    </message>
    <message>
        <source>Save to File</source>
        <translation type="obsolete">保存到文件</translation>
    </message>
    <message>
        <source>Save Value</source>
        <translation type="obsolete">保存内容</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="913"/>
        <source>Save value to file</source>
        <translation>保存内容到文件</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="10"/>
        <source>Save Raw Value to File</source>
        <translation>保存原始内容到文件</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="10"/>
        <source>Save Formatted Value to File</source>
        <translation>保存格式化后的内容到文件</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="27"/>
        <source>Save Raw Value</source>
        <translation>保存原始内容</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="27"/>
        <source>Save Formatted Value</source>
        <translation>保存格式化后的内容</translation>
    </message>
    <message>
        <source>Save raw value to file</source>
        <translation type="vanished">保存原始内容到文件</translation>
    </message>
    <message>
        <source>Save formatted value to file</source>
        <translation type="vanished">保存格式化后的内容到文件</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="51"/>
        <source>Value was saved to file:</source>
        <translation>内容已保存到文件：</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/abstractoperation.cpp" line="38"/>
        <source>Cannot connect to redis-server</source>
        <translation>无法连接到Redis服务器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="13"/>
        <source>Edit Connection Group</source>
        <translation>编辑连接组</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="17"/>
        <source>Delete Connection Group</source>
        <translation>删除连接组</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/servergroup.cpp" line="58"/>
        <source>Do you really want to delete group &lt;b&gt;with all connections&lt;/b&gt;?</source>
        <translation>真的要删除组内&lt;b&gt;所有连接&lt;/b&gt;吗？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="8"/>
        <source>Order of elements:</source>
        <translation>要素排序</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="20"/>
        <source>Default</source>
        <translation>默认</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="21"/>
        <source>Reverse</source>
        <translation>反向</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="28"/>
        <source>Start date should be less than End date</source>
        <translation>开始时间应该早于结束时间</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="136"/>
        <source>Apply filter</source>
        <translation>应用筛选条件</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="19"/>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="25"/>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="49"/>
        <source>Trial is active till</source>
        <translation>试用到</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="58"/>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="59"/>
        <source>Licensed to</source>
        <translation>授权给</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="66"/>
        <source>Subscription is active until:</source>
        <translation>订阅到期时间为：</translation>
    </message>
    <message>
        <location filename="../../modules/api/qml/SubscriptionInfo.qml" line="71"/>
        <source>Manage Subscription</source>
        <translation>管理订阅</translation>
    </message>
    <message>
        <location filename="../../qml/WelcomeTab.qml" line="30"/>
        <source>&lt;span style=&quot;font-size: 11px;&quot;&gt;Powered by awesome &lt;a href=&quot;https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty&quot;&gt;open-source software&lt;/a&gt; and &lt;a href=&quot;http://icons8.com/&quot;&gt;icons8&lt;/a&gt;.&lt;/span&gt;</source>
        <translation type="unfinished">&lt;span style=&quot;font-size: 11px;&quot;&gt;Powered by awesome &lt;a href=&quot;https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty&quot;&gt;open-source software&lt;/a&gt; and &lt;a href=&quot;http://icons8.com/&quot;&gt;icons8&lt;/a&gt;.&lt;/span&gt;</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="11"/>
        <source>Getting Started</source>
        <translation>使用入门</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="42"/>
        <source>Thank you for choosing RESP.app. Let&apos;s make your Redis experience better.</source>
        <translation>感谢您选择 RESP.app。 让我们给您的 Redis 体验更好。</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="60"/>
        <source>Connect to Redis-Server</source>
        <translation>连接到Redis服务器</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="73"/>
        <source>Read the Docs</source>
        <translation>阅读文档</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/loadmoreitem.cpp" line="12"/>
        <source>Load more keys</source>
        <translation>加载更多的键</translation>
    </message>
    <message>
        <location filename="../../qml/common/BetterMessageDialog.qml" line="24"/>
        <source>Yes</source>
        <translation>是</translation>
    </message>
    <message>
        <location filename="../../qml/common/BetterMessageDialog.qml" line="32"/>
        <source>No</source>
        <translation>否</translation>
    </message>
    <message>
        <location filename="../../qml/connections/AskSecretDialog.qml" line="19"/>
        <source>SSH Passphrase</source>
        <translation>SSH 密钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/AskSecretDialog.qml" line="21"/>
        <source>Unknown</source>
        <translation>未知</translation>
    </message>
    <message>
        <location filename="../../qml/connections/AskSecretDialog.qml" line="46"/>
        <location filename="../../qml/connections/ConnectionSettignsDialog.qml" line="717"/>
        <source>Passphrase</source>
        <translation>密钥</translation>
    </message>
    <message>
        <location filename="../../qml/connections/AskSecretDialog.qml" line="71"/>
        <source>Continue</source>
        <translation>继续</translation>
    </message>
    <message>
        <location filename="../../qml/common/ColorInput.qml" line="43"/>
        <source>Select</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/UnsupportedDataType.qml" line="24"/>
        <source>Unsupported Redis Data type </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="163"/>
        <source>Cannot delete key:

</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
