<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_TW">
<context>
    <name>QObject</name>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="351"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="388"/>
        <source>Cannot connect to cluster node %1:%2</source>
        <translation>無法連線到叢集節點 %1:%2</translation>
    </message>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="399"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="426"/>
        <source>Cannot flush db (%1): %2</source>
        <translation>無法清空資料庫 (%1): %2</translation>
    </message>
</context>
<context>
    <name>RDM</name>
    <message>
        <location filename="../../app/app.cpp" line="83"/>
        <source>Settings directory is not writable</source>
        <translation>設定儲存資料夾沒有寫入權限</translation>
    </message>
    <message>
        <location filename="../../app/app.cpp" line="85"/>
        <source>RDM can&apos;t save connections file to settings directory. Please change file permissions or restart RDM as administrator.</source>
        <translation>RDM 無法儲存設定檔。請更改檔寫入權限或者以管理員模式啟動 RDM。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="85"/>
        <source>Cannot rename key %1: %2</source>
        <translation>無法重新命名鍵 %1: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="129"/>
        <source>Cannot persist key &apos;%1&apos;. &lt;br&gt; Key does not exist or does not have an assigned TTL value</source>
        <translation>無法將鍵持久化 &apos;%1&apos; &lt;br&gt; 鍵不存在或是不會逾時</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="164"/>
        <source>Cannot parse scan response</source>
        <translation>無法解析 scan 的結果</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="324"/>
        <source>Server returned unexpected response: </source>
        <translation>伺服器返回未預期的回應: </translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="110"/>
        <source>Cannot set TTL for key %1</source>
        <translation>無法設定鍵 %1 的 TTL</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="270"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="281"/>
        <source>Cannot load rows for key %1: %2</source>
        <translation>無法載入鍵 %1 的資料: %2</translation>
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
        <location filename="../../app/models/key-models/stream.cpp" line="44"/>
        <location filename="../../app/models/key-models/stream.cpp" line="55"/>
        <source>Invalid row</source>
        <translation>無效資料</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="113"/>
        <source>Value with the same key already exists</source>
        <translation>已經存在同名的鍵</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="188"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="336"/>
        <location filename="../../app/models/treeoperations.cpp" line="154"/>
        <location filename="../../modules/console/consolemodel.cpp" line="89"/>
        <source>Connection error: </source>
        <translation>連線錯誤: </translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="136"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="136"/>
        <source>Data was loaded from server partially.</source>
        <translation>部分資料已經從伺服器載入。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="29"/>
        <source>Cannot load key %1, connection error occurred: %2</source>
        <translation>無法載入鍵 %1，連線發生錯誤: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="40"/>
        <source>Cannot load key %1 because it doesn&apos;t exist in database. Please reload connection tree and try again.</source>
        <translation>無法載入鍵 %1，資料庫中不存在該鍵，請重新載入連線樹後重試。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="67"/>
        <source>Cannot load TTL for key %1, connection error occurred: %2</source>
        <translation>無法載入鍵 %1 的 TTL 值，連線發生錯誤: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="85"/>
        <source>Cannot retrieve type of the key: </source>
        <translation>無法取得鍵的類型: </translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="59"/>
        <source>Unsupported Redis Data type %1</source>
        <translation>不支援的 Redis 資料類型 %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="132"/>
        <source>Cannot connect to server &apos;%1&apos;. Check log for details.</source>
        <translation>無法連線到伺服器 &apos;%1&apos; 。細節請查看紀錄檔。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="142"/>
        <source>Open Source version of RDM &lt;b&gt;doesn&apos;t support SSH tunneling&lt;/b&gt;.&lt;br /&gt;&lt;br /&gt; To get fully-featured application, please buy subscription on &lt;a href=&apos;https://rdm.dev/subscriptions&apos;&gt;rdm.dev&lt;/a&gt;. &lt;br/&gt;&lt;br /&gt;Every single subscription gives us funds to continue the development process and provide support to our users. &lt;br /&gt;If you have any questions please feel free to contact us at &lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt; or join &lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;.</source>
        <translation>開源版本的 RDM &lt;b&gt;不支援 SSH 隧道功能&lt;/b&gt;。&lt;br /&gt;&lt;br /&gt;若要取得完整功能的程式，請在 &lt;a href=&apos;https://rdm.dev/subscriptions&apos;&gt;rdm.dev&lt;/a&gt; 上購買訂閱。&lt;br/&gt;&lt;br /&gt;每個訂閱都是我們繼續開發以及支援使用者的原動力。&lt;br /&gt;如果你有任何問題，請聯絡 &lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt; 或是加入 &lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram 聊天群組&lt;/a&gt;。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="222"/>
        <source>Cannot load keys: %1</source>
        <translation>無法載入鍵: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="308"/>
        <source>Delete key error: %1</source>
        <translation>刪除鍵時發生錯誤:</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="428"/>
        <source>Cannot determine amount of used memory by key: %1</source>
        <translation>無法判定鍵所消耗的記憶體: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="371"/>
        <source>Cannot flush database: </source>
        <translation>無法清空資料庫: </translation>
    </message>
    <message>
        <location filename="../../modules/common/tabmodel.cpp" line="43"/>
        <source>Invalid Connection. Check connection settings.</source>
        <translation>無效連線，請檢查連線設定。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="196"/>
        <source>Live update was disabled due to exceeded keys limit. Please specify filter more carefully or change limit in settings.</source>
        <translation>由於超出載入鍵的數量限制，同步更新功能已經關閉。請設定更精確的篩選條件或更改載入限制設定。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="239"/>
        <source>Key was added. Do you want to reload keys in selected database?</source>
        <translation>已經添加鍵。需要重新載入該資料庫的鍵名嗎？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="247"/>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="141"/>
        <source>Key was added</source>
        <translation>已經插入鍵</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="255"/>
        <source>Another operation is currently in progress</source>
        <translation>另一項操作正在進行中</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="257"/>
        <source>Please wait until another operation will be finished.</source>
        <translation>請耐心等待另一項操作完成。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="268"/>
        <source>Do you really want to remove all keys from this database?</source>
        <translation>確定要刪除該資料庫裡面所有的鍵嗎？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="68"/>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="76"/>
        <source>Cannot load databases:

</source>
        <translation>無法載入資料庫:

</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="195"/>
        <source>Live update was disabled</source>
        <translation>同步更新已經禁止</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="184"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="191"/>
        <source>Rename key</source>
        <translation>重新命名鍵</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="200"/>
        <source>New name:</source>
        <translation>新名稱:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="21"/>
        <source>Total pages: </source>
        <translation>總頁數: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="45"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="223"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="292"/>
        <source>Size: </source>
        <translation>大小: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="229"/>
        <source>TTL:</source>
        <translation>TTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="234"/>
        <source>Set key TTL</source>
        <translation>設定鍵的 TTL</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="243"/>
        <source>New TTL:</source>
        <translation>新的 TTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="298"/>
        <source>Delete</source>
        <translation>刪除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="303"/>
        <source>Delete key</source>
        <translation>刪除鍵</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="38"/>
        <source>Changes are not saved</source>
        <translation>並未儲存變更</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="39"/>
        <source>Do you want to close key tab without saving changes?</source>
        <translation>要不儲存變更就關閉頁籤嗎？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="260"/>
        <source>Persist key</source>
        <translation>將鍵持久化</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="304"/>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="146"/>
        <source>Do you really want to delete this key?</source>
        <translation>確定要刪除該鍵？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="140"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="318"/>
        <source>Reload Value</source>
        <translation>重新載入鍵值</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="31"/>
        <source>Add Row</source>
        <translation>插入列</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="30"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="367"/>
        <source>Add Element to HLL</source>
        <translation>新增元素到 HHL</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="68"/>
        <source>Add</source>
        <translation>新增</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="103"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="122"/>
        <source>Delete row</source>
        <translation>刪除列</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="109"/>
        <source>The row is the last one in the key. After removing it key will be deleted.</source>
        <translation>此列資料是該鍵最後一列。刪除此列資料，該鍵將會被刪除。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="111"/>
        <source>Do you really want to remove this row?</source>
        <translation>確定要刪除此列資料嗎？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="164"/>
        <source>Search on page...</source>
        <translation>頁面搜尋...</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="191"/>
        <source>Full Search</source>
        <translation>全文搜尋</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="185"/>
        <source>Value and Console tabs related to this connection will be closed. Do you want to continue?</source>
        <translation>所有與該連線相關的鍵值對話方塊和指令操作對話方塊都將被關閉，確定要繼續嗎？</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="197"/>
        <source>Do you really want to delete connection?</source>
        <translation>確定要刪除連線？</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="13"/>
        <source>Connected to cluster.
</source>
        <translation>已連線到叢集伺服器。
</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="16"/>
        <source>Connected.
</source>
        <translation>已連線。
</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="65"/>
        <source>Switch to Pub/Sub mode. Close console tab to stop listen for messages.</source>
        <translation>切斷到 發布/訂閱 模式。關閉控制台以停止監聽訊息。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="74"/>
        <source>Subscribe error: %1</source>
        <translation>訂閱錯誤: %1</translation>
    </message>
    <message>
        <location filename="../../modules/server-stats/serverstatsmodel.cpp" line="94"/>
        <source>Server %0</source>
        <translation>伺服器 %0</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="184"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="219"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="248"/>
        <source>Can&apos;t find formatter with name: %1</source>
        <translation>找不到格式化工具: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="190"/>
        <source>Invalid callback</source>
        <translation>無效回調</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="204"/>
        <source>Cannot decode value using %1 formatter. </source>
        <translation>無法使用格式化工具解碼值 %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="233"/>
        <source>Cannot validate value using %1 formatter.</source>
        <translation>無法使用格式化工具驗證值 %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="262"/>
        <source>Cannot encode value using %1 formatter. </source>
        <translation>無法使用格式化工具編碼值 %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="26"/>
        <source>Loading key: %1 from db %2</source>
        <translation>從資料庫 %2 中載入鍵 %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="68"/>
        <source>Cannot open value tab</source>
        <translation>無法打開鍵值對話方塊</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="101"/>
        <source>Connection error. Can&apos;t open value tab. </source>
        <translation>連線錯誤，無法打開鍵值對話方塊。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="164"/>
        <source>Cannot reload key value: %1</source>
        <translation>無法重新載入鍵值: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="216"/>
        <source>Cannot load key value: %1</source>
        <translation>無法載入鍵值: %1</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="28"/>
        <source>Connect to Redis Server</source>
        <translation>連線到 Redis 伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="40"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="206"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="114"/>
        <source>Import</source>
        <translation>匯入</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="41"/>
        <location filename="../../qml/AppToolBar.qml" line="48"/>
        <source>Import Connections</source>
        <translation>匯入連線</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="58"/>
        <source>Export</source>
        <translation>匯出</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="59"/>
        <location filename="../../qml/AppToolBar.qml" line="66"/>
        <source>Export Connections</source>
        <translation>匯出連線</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="81"/>
        <source>Report issue</source>
        <translation>回報問題</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="88"/>
        <source>Documentation</source>
        <translation>說明文件</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="95"/>
        <source>Join Telegram Chat</source>
        <translation>加入 Telegram 聊天群組</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="102"/>
        <source>Follow</source>
        <translation>追隨</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="109"/>
        <source>Star on GitHub!</source>
        <translation>在 GitHub 上給個 Star</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="117"/>
        <source>Log</source>
        <translation>紀錄</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="125"/>
        <location filename="../../qml/GlobalSettings.qml" line="14"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="11"/>
        <source>New Connection Settings</source>
        <translation>新連線設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="144"/>
        <source>How to connect</source>
        <translation>如何連線</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="151"/>
        <source>Connection Settings</source>
        <translation>連線設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="180"/>
        <source>Create connection from Redis URL</source>
        <translation>以 Redis URL 建立連線</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="233"/>
        <source>Learn more about Redis URL:  </source>
        <translation>了解更多關於 Redis URL: </translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="241"/>
        <source>Connection guides</source>
        <translation>連線嚮導</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="262"/>
        <source>Local or Public Redis</source>
        <translation>本機或公開的 Redis</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="278"/>
        <source>Redis with SSL/TLS</source>
        <translation>使用 SSL/TLS 的 Redis</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="294"/>
        <source>SSH tunnel</source>
        <translation>SSH 隧道</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="310"/>
        <source>UNIX socket</source>
        <translation>UNIX socket</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="397"/>
        <source>Cannot figure out how to connect to your redis-server?</source>
        <translation>不知道如何連線到您的 Redis 伺服器嗎？</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="403"/>
        <source>&lt;a href=&quot;https://docs.rdm.dev/en/latest/quick-start/&quot;&gt;Read the Docs&lt;/a&gt;, &lt;a href=&quot;mailto:support@rdm.dev&quot;&gt;Contact Support&lt;/a&gt; or ask for help in our &lt;a href=&quot;https://t.me/RedisDesktopManager&quot;&gt;Telegram Group&lt;/a&gt;</source>
        <translation>&lt;a href=&quot;https://docs.rdm.dev/en/latest/quick-start/&quot;&gt;閱讀文件&lt;/a&gt;，&lt;a href=&quot;mailto:support@rdm.dev&quot;&gt;聯絡客服&lt;/a&gt;或是在 &lt;a href=&quot;https://t.me/RedisDesktopManager&quot;&gt;Telegram 群組&lt;/a&gt; 內請求協助。</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="415"/>
        <source>Don&apos;t have running Redis?</source>
        <translation>Redis 沒有在執行中嗎？</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="421"/>
        <source>Spin up hassle-free Redis on Digital Ocean</source>
        <translation>快速使用 Digital Ocean 上的 Redis</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="433"/>
        <source>Skip</source>
        <translation>略過</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="468"/>
        <source>Name:</source>
        <translation>名稱:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="474"/>
        <source>Connection Name</source>
        <translation>連線名稱</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="480"/>
        <source>Address:</source>
        <translation>位址:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="485"/>
        <source>redis-server host</source>
        <translation>Redis 伺服器位址</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="509"/>
        <source>(Optional) redis-server authentication password</source>
        <translation>(可選) Redis 伺服器的認證密碼</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="528"/>
        <source>Security</source>
        <translation>安全設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="559"/>
        <source>Public Key:</source>
        <translation>公鑰:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="565"/>
        <source>(Optional) Public Key in PEM format</source>
        <translation>(可選) PEM 格式的公鑰</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="567"/>
        <source>Select public key in PEM format</source>
        <translation>選擇 PEM 格式的公鑰</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="578"/>
        <source>(Optional) Private Key in PEM format</source>
        <translation>(可選) PEM 格式的私鑰</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="580"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="674"/>
        <source>Select private key in PEM format</source>
        <translation>選擇 PEM 格式的私鑰</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="585"/>
        <source>Authority:</source>
        <translation>授權證書:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="591"/>
        <source>(Optional) Authority in PEM format</source>
        <translation>(可選) PEM 格式的授權證書</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="593"/>
        <source>Select authority file in PEM format</source>
        <translation>選擇 PEM 格式的授權證書</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="612"/>
        <source>SSH Tunnel</source>
        <translation>SSH 隧道</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="631"/>
        <source>SSH Address:</source>
        <translation>SSH 位址:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="635"/>
        <source>Remote Host with SSH server</source>
        <translation>SSH 遠端伺服器位址</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="643"/>
        <source>SSH User:</source>
        <translation>SSH 使用者:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="649"/>
        <source>Valid SSH User Name</source>
        <translation>有效的 SSH 使用者名稱</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="572"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="656"/>
        <source>Private Key</source>
        <translation>私鑰</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="672"/>
        <source>Path to Private Key in PEM format</source>
        <translation>PEM 格式私鑰路徑</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="682"/>
        <source>&lt;b&gt;Tip:&lt;/b&gt; Use &lt;code&gt;⌘ + Shift + .&lt;/code&gt; to show hidden files and folders in dialog</source>
        <translation>&lt;b&gt;提示: &lt;/b&gt; &lt;code&gt;⌘ + Shift + .&lt;/code&gt; 可以顯示隱藏的檔案與資料夾</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="687"/>
        <source>Password</source>
        <translation>密碼</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="698"/>
        <source>SSH User Password</source>
        <translation>SSH 使用者密碼</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="709"/>
        <source>Enable TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</source>
        <translation>啟用 TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="156"/>
        <source>Advanced Settings</source>
        <translation>進階設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="496"/>
        <source>For better network performance please use 127.0.0.1</source>
        <translation>使用 127.0.0.1 以提高網路性能</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="514"/>
        <source>Username:</source>
        <translation>使用者名稱:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="520"/>
        <source>(Optional) redis-server authentication username (Redis &gt;6.0)</source>
        <translation>(可選) Redis 伺服器認證使用者名稱 (Redis &gt;6.0)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="539"/>
        <source>SSL / TLS</source>
        <translation>SSL / TLS</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="598"/>
        <source>Enable strict mode:</source>
        <translation>啟用嚴格模式:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="746"/>
        <source>Keys loading</source>
        <translation>鍵的載入</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="750"/>
        <source>Default filter:</source>
        <translation>預設篩選器:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="756"/>
        <source>Pattern which defines loaded keys from redis-server</source>
        <translation>指定載入鍵名運算式:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="762"/>
        <source>Namespace Separator:</source>
        <translation>命名空間的分隔符號:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="769"/>
        <source>Separator used for namespace extraction from keys</source>
        <translation>從鍵名中提取命名空間用的分隔符號</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="775"/>
        <source>Timeouts &amp; Limits</source>
        <translation>超時 &amp; 限制</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="779"/>
        <source>Connection Timeout (sec):</source>
        <translation>連線逾時 (秒):</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="792"/>
        <source>Execution Timeout (sec):</source>
        <translation>執行超時 (秒):</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="803"/>
        <source>Databases discovery limit:</source>
        <translation>資料庫探索上限:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="817"/>
        <source>Cluster</source>
        <translation>叢集</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="821"/>
        <source>Change host on cluster redirects:</source>
        <translation>在叢集重定向後改變 host :</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="877"/>
        <source>Invalid settings detected!</source>
        <translation>檢測到無效的設定！</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="848"/>
        <source>Test Connection</source>
        <translation>測試連線</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="885"/>
        <location filename="../../qml/GlobalSettings.qml" line="339"/>
        <location filename="../../qml/common/SaveToFileButton.qml" line="114"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="715"/>
        <source>OK</source>
        <translation>確定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="899"/>
        <location filename="../../qml/GlobalSettings.qml" line="351"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="399"/>
        <location filename="../../qml/common/BetterDialog.qml" line="44"/>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="62"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="107"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="91"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="268"/>
        <source>Cancel</source>
        <translation>取消</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="47"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="52"/>
        <source>Application will be restarted to apply these settings.</source>
        <translation>程式將會重新啟動以套用此設定</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="72"/>
        <source>Language</source>
        <translation>語言</translation>
    </message>
    <message>
        <source>Application will be restarted to apply this setting.</source>
        <translation type="vanished">程式將會重新啟動以套用新的設定</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="84"/>
        <location filename="../../qml/GlobalSettings.qml" line="175"/>
        <source>Font</source>
        <translation>字體</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="97"/>
        <location filename="../../qml/GlobalSettings.qml" line="188"/>
        <source>Font Size</source>
        <translation>字體大小</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="110"/>
        <location filename="../../qml/GlobalSettings.qml" line="124"/>
        <source>Dark Mode</source>
        <translation>深色模式</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="202"/>
        <source>Maximum Formatted Value Size</source>
        <translation>最大格式化長度</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="203"/>
        <source>Size in bytes</source>
        <translation>長度（位元組）</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="138"/>
        <source>Use system proxy settings</source>
        <translation>使用系統的代理設定</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="150"/>
        <source>Use system proxy only for HTTP(S) requests</source>
        <translation>只為 HTTP(S) 使用系統的代理</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="156"/>
        <source>Value Editor</source>
        <translation>值編輯器</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="209"/>
        <source>Connections Tree</source>
        <translation>連線列表</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="227"/>
        <source>Show namespaced keys on top</source>
        <translation>置頂有命名空間的鍵</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="237"/>
        <source>Reopen namespaces on reload</source>
        <translation>重新載入時重新打開命名空間</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="238"/>
        <source>(Disable to improve treeview performance)</source>
        <translation>(停用樹狀檢視以提高性能)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="248"/>
        <source>Show only last part for namespaced keys</source>
        <translation>對有命名空間的鍵只顯示最後一部分</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="260"/>
        <source>Live update maximum allowed keys</source>
        <translation>同步更新最大允許鍵數量</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="272"/>
        <source>Live update interval (in seconds)</source>
        <translation>同步更新時間 (秒)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="282"/>
        <source>External Value View Formatters</source>
        <translation>外部的值格式化工具</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="291"/>
        <source>Formatters path: %0</source>
        <translation>格式化工具路徑: %0</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="308"/>
        <source>Name</source>
        <translation>名稱</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="313"/>
        <location filename="../../qml/WelcomeTab.qml" line="30"/>
        <source>Version</source>
        <translation>版本</translation>
    </message>
    <message>
        <source>Explore RDM</source>
        <translation type="vanished">探索 RDM</translation>
    </message>
    <message>
        <source>Before using RDM take a look on the %1</source>
        <translation type="vanished">在使用 RDM 之前，您可以先看看 %1</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="857"/>
        <source>Quick Start Guide</source>
        <translation>快速入門指南</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="111"/>
        <source>Successful connection to redis-server</source>
        <translation>成功連線到 Redis 伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="114"/>
        <source>Can&apos;t connect to redis-server</source>
        <translation>無法連線到 Redis 伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="282"/>
        <source>Add Group</source>
        <translation>新增分組</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="296"/>
        <source>Regroup connections</source>
        <translation>重組連線</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="318"/>
        <source>Exit Regroup Mode</source>
        <translation>離開分組模式</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="10"/>
        <source>Bulk Operations Manager</source>
        <translation>批次操作管理器</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="63"/>
        <source>Invalid RDB path</source>
        <translation>無效的 RDB 路徑</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="63"/>
        <source>Please specify valid path to RDB file</source>
        <translation>請指定有效的 RDB 檔案</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="85"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="86"/>
        <source>Delete keys</source>
        <translation>刪除鍵</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="95"/>
        <source>Set TTL</source>
        <translation>設定 TTL</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="103"/>
        <source>Copy keys to another database</source>
        <translation>複製鍵到其他資料庫</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="104"/>
        <source>Copy keys</source>
        <translation>複製鍵</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="113"/>
        <source>Import data from rdb file</source>
        <translation>從 RDB 檔案中匯入資料</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="148"/>
        <source>Redis Server:</source>
        <translation>Redis 伺服器:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="160"/>
        <source>Database number:</source>
        <translation>資料庫編號:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="180"/>
        <source>Path to RDB file:</source>
        <translation>RDB 檔案的路徑:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="202"/>
        <source>Select DB in RDB file:</source>
        <translation>選擇 RDB 檔案中的資料庫:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="222"/>
        <source>Key pattern:</source>
        <translation>鍵名運算式:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="222"/>
        <source>Import keys that match &lt;b&gt;regex&lt;/b&gt;:</source>
        <translation>匯入符合&lt;b&gt;正規表達式&lt;/b&gt;的鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="269"/>
        <source>Destination Redis Server:</source>
        <translation>目標 Redis 伺服器:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="280"/>
        <source>Destination Redis Server Database Index:</source>
        <translation>目標資料庫編號:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="317"/>
        <source>Show matched keys</source>
        <translation>顯示符合的鍵</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="317"/>
        <source>Show Affected keys</source>
        <translation>顯示受影響的鍵</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="340"/>
        <source>Affected keys:</source>
        <translation>受影響的鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="340"/>
        <source>Matched keys:</source>
        <translation>符合的鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="365"/>
        <source>Bulk Operation finished.</source>
        <translation>批次操作完成。</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="370"/>
        <source>Bulk Operation finished with errors</source>
        <translation>批次操作完成但途中曾發生錯誤</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="422"/>
        <source>Processed: </source>
        <translation>已處理: </translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="424"/>
        <source>Getting list of affected keys...</source>
        <translation>正在取得受影響的鍵的清單...</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="489"/>
        <source>Confirmation</source>
        <translation>確認</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="490"/>
        <source>Do you really want to perform bulk operation?</source>
        <translation>確認要執行批次操作？</translation>
    </message>
    <message>
        <source>Sign in with rdm.dev account</source>
        <translation type="vanished">以 rdm.dev 的帳號登入</translation>
    </message>
    <message>
        <source>Renew your subscription</source>
        <translation type="vanished">續期您的訂閱</translation>
    </message>
    <message>
        <source>Your trial has ended.</source>
        <translation type="vanished">您的試用已經到期</translation>
    </message>
    <message>
        <source>You have no active subscription</source>
        <translation type="vanished">您沒有可用的訂閱</translation>
    </message>
    <message>
        <source>No internet connection</source>
        <translation type="vanished">無網絡連線</translation>
    </message>
    <message>
        <source>Your trial has ended</source>
        <translation type="vanished">您的試用已結束</translation>
    </message>
    <message>
        <source>To use this version you need to renew your subscription.</source>
        <translation type="vanished">您必須續期訂閱已繼續使用此版本。</translation>
    </message>
    <message>
        <source>Please make sure that RDM is not blocked by a firewall and you have an internet connection.</source>
        <translation type="vanished">請確保 RDM 沒有被防火牆阻擋，並且網絡連線正常。</translation>
    </message>
    <message>
        <source>If you’re behind a proxy please enable </source>
        <translation type="vanished">如果您處於代理之中，請啟用 </translation>
    </message>
    <message>
        <source> option before sign-in.</source>
        <translation type="vanished"> 選項（在登入前）。</translation>
    </message>
    <message>
        <source>Please purchase a subscription to continue using RDM.</source>
        <translation type="vanished">請購買訂閱以繼續使用 RDM 。</translation>
    </message>
    <message>
        <source>If you have any questions please contact support </source>
        <translation type="vanished">如果您有任何問題，請聯絡客服 </translation>
    </message>
    <message>
        <source>Renew Subscription</source>
        <translation type="vanished">續期訂閱</translation>
    </message>
    <message>
        <source>Buy Subscription</source>
        <translation type="vanished">購買訂閱</translation>
    </message>
    <message>
        <source>Try Again</source>
        <translation type="vanished">重試</translation>
    </message>
    <message>
        <source>Email:</source>
        <translation type="vanished">Email:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="503"/>
        <source>Password:</source>
        <translation>密碼:</translation>
    </message>
    <message>
        <location filename="../../qml/common/PasswordInput.qml" line="20"/>
        <source>Show password</source>
        <translation>顯示密碼</translation>
    </message>
    <message>
        <source>Forgot password?</source>
        <translation type="vanished">忘記密碼？</translation>
    </message>
    <message>
        <source>Sign In</source>
        <translation type="vanished">登入</translation>
    </message>
    <message>
        <source>Please enter email &amp; password to sign in.</source>
        <translation type="vanished">請輸入 email 與密碼登入。</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/BetterTreeView.qml" line="214"/>
        <source> (Removed)</source>
        <translation> (已移除)</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="76"/>
        <source>Open Keys Filter</source>
        <translation>打開鍵篩選器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="80"/>
        <source>Reload Keys in Database</source>
        <translation>重新載入資料庫中的鍵</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="84"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="29"/>
        <source>Add New Key</source>
        <translation>新增鍵</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="90"/>
        <source>Disable Live Update</source>
        <translation>停用同步更新</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="90"/>
        <source>Enable Live Update</source>
        <translation>啟用同步更新</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="94"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="23"/>
        <source>Open Console</source>
        <translation>打開控制台</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="97"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="31"/>
        <source>Analyze Used Memory</source>
        <translation>分析記憶體用量</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="99"/>
        <source>Bulk Operations</source>
        <translation>批次操作</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="120"/>
        <source>Flush Database</source>
        <translation>清空資料庫</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="123"/>
        <source>Delete keys with filter</source>
        <translation>使用篩選器來刪除鍵</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="94"/>
        <location filename="../../qml/connections-tree/menu/database.qml" line="126"/>
        <source>Set TTL for multiple keys</source>
        <translation>為多個鍵設定 TTL</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="129"/>
        <source>Copy keys from this database to another</source>
        <translation>從此資料庫中複製鍵到另一個資料庫</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="132"/>
        <source>Import keys from RDB file</source>
        <translation>從 RDB 檔案中匯入鍵</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="135"/>
        <source>Back</source>
        <translation>返回</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="21"/>
        <source>Copy Key Name</source>
        <translation>複製鍵</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="28"/>
        <source>Reload Namespace</source>
        <translation>重新載入命名空間</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="30"/>
        <source>Copy Namespace Pattern</source>
        <translation>複製命名空間運算式</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="32"/>
        <source>Delete Namespace</source>
        <translation>刪除命名空間</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="70"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="23"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="13"/>
        <source>Disconnect</source>
        <translation>中斷連線</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="19"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="239"/>
        <source>Server Info</source>
        <translation>伺服器資訊</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="27"/>
        <source>Reload Server</source>
        <translation>重新載入伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="31"/>
        <source>Unload All Data</source>
        <translation>卸載所有資料</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="11"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="35"/>
        <source>Edit Connection Settings</source>
        <translation>編輯連線設定</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="39"/>
        <source>Duplicate Connection</source>
        <translation>複製連線</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="43"/>
        <source>Delete Connection</source>
        <translation>刪除連線</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="21"/>
        <source>Connecting...</source>
        <translation>連線中...</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="193"/>
        <source>Clear</source>
        <translation>清除</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="233"/>
        <source>Arguments</source>
        <translation>參數</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="323"/>
        <location filename="../../qml/console/RedisConsole.qml" line="239"/>
        <source>Description</source>
        <translation>描述</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="245"/>
        <source>Available since</source>
        <translation>可用自</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="296"/>
        <source>Close</source>
        <translation>關閉</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="109"/>
        <source>Redis Version</source>
        <translation>Redis 版本</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="124"/>
        <source>Used memory</source>
        <translation>已使用記憶體</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="139"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="247"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="351"/>
        <source>Clients</source>
        <translation>連線數</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="154"/>
        <source>Commands Processed</source>
        <translation>已執行指令</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="171"/>
        <source>Uptime</source>
        <translation>上線時間</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="186"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="488"/>
        <source>Total Keys</source>
        <translation>鍵總數</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="201"/>
        <source>Hit Ratio</source>
        <translation>命中率</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="221"/>
        <source> day(s)</source>
        <translation> 天</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="235"/>
        <source>Info</source>
        <translation>資訊</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="299"/>
        <source>Commands Per Second</source>
        <translation>每秒指令數</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="313"/>
        <source>Ops/s</source>
        <translation>操作/秒</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="337"/>
        <source>Connected Clients</source>
        <translation>已連線的客戶端</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="375"/>
        <source>Memory Usage</source>
        <translation>記憶體佔用</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="387"/>
        <source>Mb</source>
        <translation>Mb</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="416"/>
        <source>Network Input</source>
        <translation>網路輸入</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="428"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="464"/>
        <source>Kb/s</source>
        <translation>Kb/s</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="452"/>
        <source>Network Output</source>
        <translation>網路輸出</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="502"/>
        <source>Keys</source>
        <translation>鍵數量</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="585"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="693"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="759"/>
        <source>Auto Refresh</source>
        <translation>自動重整</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="619"/>
        <source>Property</source>
        <translation>屬性</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="625"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="22"/>
        <source>Value</source>
        <translation>值</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="847"/>
        <source>Subscribe in Console</source>
        <translation>在控制台中訂閱</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="243"/>
        <source>Slowlog</source>
        <translation>慢紀錄</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="252"/>
        <source>Pub/Sub Channels</source>
        <translation>發布/訂閱 頻道</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="817"/>
        <source>Enable</source>
        <translation>啟用</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="836"/>
        <source>Channel Name</source>
        <translation>頻道名稱</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="317"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="708"/>
        <source>Command</source>
        <translation>指令</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="725"/>
        <source>Processed at</source>
        <translation>已處理於</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="740"/>
        <source>Execution Time (μs)</source>
        <translation>執行時間 (微秒)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="774"/>
        <source>Client Address</source>
        <translation>客戶端位址</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="780"/>
        <source>Age (sec)</source>
        <translation>連線時長 (秒)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="786"/>
        <source>Idle</source>
        <translation>閒置</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="792"/>
        <source>Flags</source>
        <translation>旗標</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="798"/>
        <source>Current Database</source>
        <translation>當前資料庫</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="9"/>
        <source>Add New Key to </source>
        <translation>新增鍵到</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="25"/>
        <location filename="../../qml/value-editor/editors/HashItemEditor.qml" line="17"/>
        <source>Key:</source>
        <translation>鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="35"/>
        <source>Type:</source>
        <translation>類型:</translation>
    </message>
    <message>
        <location filename="../../qml/common/BetterDialog.qml" line="39"/>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="47"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="71"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="255"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="437"/>
        <source>Save</source>
        <translation>儲存</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="11"/>
        <source>Edit Connections Group</source>
        <translation>編輯連線的群組</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="11"/>
        <source>Add New Connections Group</source>
        <translation>新增連線群組</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="30"/>
        <source>Group Name:</source>
        <translation>群組名稱:</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="71"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="118"/>
        <location filename="../../qml/value-editor/editors/formatters/ValueFormatters.qml" line="219"/>
        <source>Error</source>
        <translation>錯誤</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="12"/>
        <source>Page</source>
        <translation>頁</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="54"/>
        <source>Enter valid value</source>
        <translation>請輸入有效的值</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="264"/>
        <source>Formatting error</source>
        <translation>格式化錯誤</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="269"/>
        <source>Unknown formatter error (Empty response)</source>
        <translation>未知格式化錯誤 (沒有回應)</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="297"/>
        <source>[Binary]</source>
        <translation>[二進位制內容]</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="300"/>
        <source> [Compressed: </source>
        <translation>[被壓縮的: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="385"/>
        <source>Copy to Clipboard</source>
        <translation>複製到剪貼簿</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="438"/>
        <source>Save Changes</source>
        <translation>儲存變更</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="542"/>
        <source>Search string</source>
        <translation>搜尋字串</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="556"/>
        <source>Find Next</source>
        <translation>尋找下一筆</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="556"/>
        <source>Find</source>
        <translation>尋找</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="588"/>
        <source>Regex</source>
        <translation>正規表示式</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="574"/>
        <source>Cannot find more results</source>
        <translation>找不到更多結果</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="575"/>
        <source>Cannot find any results</source>
        <translation>找不到任何結果</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="711"/>
        <source>Binary value is too large to display</source>
        <translation>二進位制內容過長而無法顯示</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="306"/>
        <source>View as:</source>
        <translation>以...開啟:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="343"/>
        <source>Large value (&gt;150kB). Formatters are not available.</source>
        <translation>內容過大 (&gt;150kB) 無法格式化。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="18"/>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="30"/>
        <source>Score</source>
        <translation>分數</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="191"/>
        <source>Path to dump.rdb file</source>
        <translation>dump.rdb 的路徑</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="193"/>
        <source>Select dump.rdb</source>
        <translation>選擇 dump.rdb</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="18"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="61"/>
        <source>Value (represented as JSON object)</source>
        <translation>值 (以 JSON 物件表示)</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/listkey.cpp" line="127"/>
        <source>The row has been changed on server.Reload and try again.</source>
        <translation>此列資料已在伺服器上被修改，請重新載入後再試一次。</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/bulkoperationsmanager.cpp" line="122"/>
        <source>Failed to perform actions on %1 keys. </source>
        <translation>無法在鍵 %1 上執行動作。 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="12"/>
        <source>Cannot copy key </source>
        <translation>無法複製鍵 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="119"/>
        <source>Source connection error</source>
        <translation>來源連線錯誤</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="131"/>
        <source>Target connection error</source>
        <translation>目標連線錯誤</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/deleteoperation.cpp" line="11"/>
        <source>Cannot remove key </source>
        <translation>無法刪除鍵 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="16"/>
        <source>Cannot execute command </source>
        <translation>無法執行指令 </translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/ttloperation.cpp" line="10"/>
        <source>Cannot set TTL for key </source>
        <translation>無法設定 TTL 給鍵 </translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/abstractnamespaceitem.cpp" line="157"/>
        <source>Your redis-server doesn&apos;t support &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; commands.</source>
        <translation>你的 Redis 伺服器不支援 &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; 指令。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="136"/>
        <source>Key was added. Do you want to reload keys in selected namespace?</source>
        <translation>已新增鍵。你想要重新載入命名空間中的鍵嗎？</translation>
    </message>
    <message>
        <source>Network is not accessible. Please ensure that you have internet access and try again.</source>
        <translation type="vanished">無法存取網路。請確認您可以存取網路後重新再試。</translation>
    </message>
    <message>
        <source>Invalid login or password</source>
        <translation type="vanished">無效的登入資料</translation>
    </message>
    <message>
        <source>Too many requests from your IP</source>
        <translation type="vanished">您的 IP 發起過多的請求</translation>
    </message>
    <message>
        <source>Unknown error. Status code %1</source>
        <translation type="vanished">未知錯誤。狀態碼 %1</translation>
    </message>
    <message>
        <source>Cannot parse server reply</source>
        <translation type="vanished">無法解析伺服器回應</translation>
    </message>
    <message>
        <source>Cannot validate token</source>
        <translation type="vanished">無法驗證權杖</translation>
    </message>
    <message>
        <source>Cannot login - %1. &lt;br/&gt; Please try again or contact  &lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt;</source>
        <translation type="vanished">無法登入 - %1 。&lt;br/&gt; 請再試一次或聯絡 &lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt;</translation>
    </message>
    <message>
        <source>Cannot save the update. Disk is full or download folder is not writable.</source>
        <translation type="vanished">無法保存更新檔，可能是硬碟已滿或是資料夾無法寫入。</translation>
    </message>
    <message>
        <source>Download was canceled</source>
        <translation type="vanished">下載已被取消</translation>
    </message>
    <message>
        <source>Network error</source>
        <translation type="vanished">網路錯誤</translation>
    </message>
    <message>
        <location filename="../../qml/common/FilePathInput.qml" line="27"/>
        <source>Select File</source>
        <translation>選擇檔案</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="723"/>
        <source>Save value to file</source>
        <translation>儲存值到檔案</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="9"/>
        <source>Save Raw Value to File</source>
        <translation>儲存原始值到檔案</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="9"/>
        <source>Save Formatted Value to File</source>
        <translation>儲存格式化的值到檔案</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="26"/>
        <source>Save Raw Value</source>
        <translation>儲存原始值</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="26"/>
        <source>Save Formatted Value</source>
        <translation>儲存格式化的值</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="50"/>
        <source>Save raw value to file</source>
        <translation>儲存原始值到檔案</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="50"/>
        <source>Save formatted value to file</source>
        <translation>儲存格式化的值到檔案</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="71"/>
        <source>Value was saved to file:</source>
        <translation>已儲存值到檔案:</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/abstractoperation.cpp" line="34"/>
        <source>Cannot connect to redis-server</source>
        <translation>無法連線到 Redis 伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="12"/>
        <source>Edit Connection Group</source>
        <translation>編輯連線群組</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="16"/>
        <source>Delete Connection Group</source>
        <translation>刪除連線群組</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/servergroup.cpp" line="58"/>
        <source>Do you really want to delete group &lt;b&gt;with all connections&lt;/b&gt;?</source>
        <translation>您真的要刪除群組&lt;b&gt;以及其中的連線&lt;/b&gt;嗎？</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="8"/>
        <source>Order of elements:</source>
        <translation>元素排序:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="20"/>
        <source>Default</source>
        <translation>預設</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="21"/>
        <source>Reverse</source>
        <translation>倒序</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="28"/>
        <source>Start date should be less than End date</source>
        <translation>起始日期必須早於終止日期</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="134"/>
        <source>Apply filter</source>
        <translation>套用篩選器</translation>
    </message>
    <message>
        <source>Trial is active till</source>
        <translation type="vanished">試用到</translation>
    </message>
    <message>
        <source>Licensed to</source>
        <translation type="vanished">授權給</translation>
    </message>
    <message>
        <source>Subscription is active until:</source>
        <translation type="vanished">訂閱到:</translation>
    </message>
    <message>
        <source>Manage Subscription</source>
        <translation type="vanished">管理訂閱</translation>
    </message>
    <message>
        <location filename="../../qml/WelcomeTab.qml" line="31"/>
        <source>&lt;span style=&quot;font-size: 11px;&quot;&gt;Powered by awesome &lt;a href=&quot;https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty&quot;&gt;open-source software&lt;/a&gt; and &lt;a href=&quot;http://icons8.com/&quot;&gt;icons8&lt;/a&gt;.&lt;/span&gt;</source>
        <translation>&lt;span style=&quot;font-size: 11px;&quot;&gt;由卓越的 &lt;a href=&quot;https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty&quot;&gt;開源軟體&lt;/a&gt; 以及 &lt;a href=&quot;http://icons8.com/&quot;&gt;icons8&lt;/a&gt; 驅動。&lt;/span&gt;</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="11"/>
        <source>Getting Started</source>
        <translation>入門指南</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="41"/>
        <source>Thank you for choosing RDM. Let&apos;s make your Redis experience better.</source>
        <translation>感謝您選用 RDM 。我們一起讓 Redis 有更好的使用體驗吧！</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="59"/>
        <source>Connect to Redis-Server</source>
        <translation>連線到 Redis 伺服器</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="72"/>
        <source>Read the Docs</source>
        <translation>閱讀文件</translation>
    </message>
</context>
</TS>
