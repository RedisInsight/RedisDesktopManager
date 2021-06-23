<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ja_JP">
<context>
    <name>QObject</name>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="351"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="388"/>
        <source>Cannot connect to cluster node %1:%2</source>
        <translation>クラスタノードに書き込みできません&#x3000;%1:%2</translation>
    </message>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="399"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="426"/>
        <source>Cannot flush db (%1): %2</source>
        <translation>DBをフラッシュできません&#x3000;(%1): %2</translation>
    </message>
</context>
<context>
    <name>RDM</name>
    <message>
        <location filename="../../app/app.cpp" line="83"/>
        <source>Settings directory is not writable</source>
        <translation>設定したディレクトリは書き込みできません</translation>
    </message>
    <message>
        <location filename="../../app/app.cpp" line="85"/>
        <source>RDM can&apos;t save connections file to settings directory. Please change file permissions or restart RDM as administrator.</source>
        <translation>RDMは設定ディレクトリに接続情報を保存できませんでした。ファイルのパーミッションを変更するか、RDMを管理者として再起動してください。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="164"/>
        <source>Cannot parse scan response</source>
        <translation>スキャンのレスポンスをパースできません</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="324"/>
        <source>Server returned unexpected response: </source>
        <translation>サーバが不正なレスポンスを返しました:</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="110"/>
        <source>Cannot set TTL for key %1</source>
        <translation>キー%1にTTLを設定できません</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="85"/>
        <source>Cannot rename key %1: %2</source>
        <translation>キー%1を変更できません: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="129"/>
        <source>Cannot persist key &apos;%1&apos;. &lt;br&gt; Key does not exist or does not have an assigned TTL value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="270"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="281"/>
        <source>Cannot load rows for key %1: %2</source>
        <translation>キー%1のROWを読むことができません: %2</translation>
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
        <translation>不正なROWです</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="113"/>
        <source>Value with the same key already exists</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="188"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="336"/>
        <location filename="../../app/models/treeoperations.cpp" line="154"/>
        <location filename="../../modules/console/consolemodel.cpp" line="89"/>
        <source>Connection error: </source>
        <translation>接続エラー: </translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="136"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="136"/>
        <source>Data was loaded from server partially.</source>
        <translation>サーバからデータが部分的にロードされました。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="29"/>
        <source>Cannot load key %1, connection error occurred: %2</source>
        <translation>キー%1を読めません。接続エラーが発生しました: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="40"/>
        <source>Cannot load key %1 because it doesn&apos;t exist in database. Please reload connection tree and try again.</source>
        <translation>データベースに存在しないためキー%1を読めません。接続ツリーをリロードしてから改めて試してください。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="67"/>
        <source>Cannot load TTL for key %1, connection error occurred: %2</source>
        <translation>キー%1のTTLがロードできません。接続エラーが発生しました: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="85"/>
        <source>Cannot retrieve type of the key: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="59"/>
        <source>Unsupported Redis Data type %1</source>
        <translation>サポートしていないRedisのデータ型です&#x3000;%1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="132"/>
        <source>Cannot connect to server &apos;%1&apos;. Check log for details.</source>
        <translation>サーバ&apos;%1&apos;に接続できません。詳細はログを確認してください。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="142"/>
        <source>Open Source version of RDM &lt;b&gt;doesn&apos;t support SSH tunneling&lt;/b&gt;.&lt;br /&gt;&lt;br /&gt; To get fully-featured application, please buy subscription on &lt;a href=&apos;https://rdm.dev/subscriptions&apos;&gt;rdm.dev&lt;/a&gt;. &lt;br/&gt;&lt;br /&gt;Every single subscription gives us funds to continue the development process and provide support to our users. &lt;br /&gt;If you have any questions please feel free to contact us at &lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt; or join &lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;.</source>
        <translation>オープンソース版のRedis Desktop ManagerはSSHトンネルをサポートして&lt;b&gt;いません&lt;/b&gt;。&lt;br /&gt;&lt;br /&gt;全ての機能を有効にするには&lt;a href=&apos;https://rdm.dev/subscriptions&apos;&gt;rdm.dev&lt;/a&gt;でサブスクリプションに加入してください。&lt;br/&gt;&lt;br /&gt;ご加入の皆様からいただきました資金により、私たちは開発を続けることができ、皆様をサポートできます。&lt;br /&gt;もしご質問がございましたらお気軽に&lt;a href=&apos;mailto:support@rdm.dev&apos;&gt;support@rdm.dev&lt;/a&gt;または&lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;までお問い合わせください。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="222"/>
        <source>Cannot load keys: %1</source>
        <translation>キーをロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="308"/>
        <source>Delete key error: %1</source>
        <translation>キー削除エラー: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="428"/>
        <source>Cannot determine amount of used memory by key: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="371"/>
        <source>Cannot flush database: </source>
        <translation>データベースをフラッシュできません: </translation>
    </message>
    <message>
        <location filename="../../modules/common/tabmodel.cpp" line="43"/>
        <source>Invalid Connection. Check connection settings.</source>
        <translation>不正な接続です。接続の設定を確認してください。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="196"/>
        <source>Live update was disabled due to exceeded keys limit. Please specify filter more carefully or change limit in settings.</source>
        <translation>キーの上限を超えているためライブアップデートは無効です。より適切なフィルタを指定するか、または設定で上限を変更してください。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="239"/>
        <source>Key was added. Do you want to reload keys in selected database?</source>
        <translation>キーを追加しました。選択したデータベースのキーをリロードしますか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="247"/>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="141"/>
        <source>Key was added</source>
        <translation>キーを追加しました。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="255"/>
        <source>Another operation is currently in progress</source>
        <translation>別の処理が動作しています。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="257"/>
        <source>Please wait until another operation will be finished.</source>
        <translation>処理が終わるまでお待ちください。</translation>
    </message>
    <message>
        <source>Please wait until another operation will be finised.</source>
        <translation type="vanished">処理が終わるまでお待ちください。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="268"/>
        <source>Do you really want to remove all keys from this database?</source>
        <translation>このデータベースから全てのキーを削除しても本当によろしいですか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="68"/>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="76"/>
        <source>Cannot load databases:

</source>
        <translation>データベースをロードできません:</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="195"/>
        <source>Live update was disabled</source>
        <translation>ライブ・アップデートは無効です</translation>
    </message>
    <message>
        <source>Live update was disabled due to exceeded keys limit. Please specify filter more carrfully or change limit in settings.</source>
        <translation type="vanished">キーの上限を超えているためライブアップデートは無効です。より適切なフィルタを指定するか、または設定で上限を変更してください。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="184"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="191"/>
        <source>Rename key</source>
        <translation>キー名の変更</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="200"/>
        <source>New name:</source>
        <translation>新しい名前:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="21"/>
        <source>Total pages: </source>
        <translation>総ページ数: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="45"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="223"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="292"/>
        <source>Size: </source>
        <translation>サイズ: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="229"/>
        <source>TTL:</source>
        <translation>TTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="234"/>
        <source>Set key TTL</source>
        <translation>キーのTTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="243"/>
        <source>New TTL:</source>
        <translation>新しいTTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="298"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="303"/>
        <source>Delete key</source>
        <translation>キーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="38"/>
        <source>Changes are not saved</source>
        <translation>変更は保存されていません</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="39"/>
        <source>Do you want to close key tab without saving changes?</source>
        <translation>変更を保存せずにキーのタブを閉じますか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="260"/>
        <source>Persist key</source>
        <translation>永続化キー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="304"/>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="146"/>
        <source>Do you really want to delete this key?</source>
        <translation>このキーを本当に削除してもよろしいですか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="140"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="318"/>
        <source>Reload Value</source>
        <translation>値をリロード</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="31"/>
        <source>Add Row</source>
        <translation>ROWを追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="30"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="367"/>
        <source>Add Element to HLL</source>
        <translation>HLLに要素を追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="68"/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="103"/>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="122"/>
        <source>Delete row</source>
        <translation>ROWを削除</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="109"/>
        <source>The row is the last one in the key. After removing it key will be deleted.</source>
        <translation>このROWはこのキーの最後の1つです。削除後はキーも削除されます。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="111"/>
        <source>Do you really want to remove this row?</source>
        <translation>このROWを本当に削除しますか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="164"/>
        <source>Search on page...</source>
        <translation>ページを検索...</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTableActions.qml" line="191"/>
        <source>Full Search</source>
        <translation>すべて検索</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="185"/>
        <source>Value and Console tabs related to this connection will be closed. Do you want to continue?</source>
        <translation>この接続に関連する値とコンソールのタブを閉じます。続行しますか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="197"/>
        <source>Do you really want to delete connection?</source>
        <translation>接続を本当に削除しますか?</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="13"/>
        <source>Connected to cluster.
</source>
        <translation>クラスタに接続しました。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="16"/>
        <source>Connected.
</source>
        <translation>接続しました。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="65"/>
        <source>Switch to Pub/Sub mode. Close console tab to stop listen for messages.</source>
        <translation>Pub/Subモードに変更する。メッセージを見るのをやめるにはコンソールのタブを閉じてください。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="74"/>
        <source>Subscribe error: %1</source>
        <translation>サブスクライブエラー: %1</translation>
    </message>
    <message>
        <location filename="../../modules/server-stats/serverstatsmodel.cpp" line="94"/>
        <source>Server %0</source>
        <translation>サーバ %0</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="184"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="219"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="248"/>
        <source>Can&apos;t find formatter with name: %1</source>
        <translation>指定したフォーマッタが見つかりません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="190"/>
        <source>Invalid callback</source>
        <translation>不正なコールバックです</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="204"/>
        <source>Cannot decode value using %1 formatter. </source>
        <translation>フォーマッタ%1で値をデコードすることができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="233"/>
        <source>Cannot validate value using %1 formatter.</source>
        <translation>フォーマッタ%1で値を検証することができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="262"/>
        <source>Cannot encode value using %1 formatter. </source>
        <translation>フォーマッタ%1で値をエンコードすることができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="26"/>
        <source>Loading key: %1 from db %2</source>
        <translation>データをロード: DB %2 の %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="68"/>
        <source>Cannot open value tab</source>
        <translation>値タブを開くことができません</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="101"/>
        <source>Connection error. Can&apos;t open value tab. </source>
        <translation>接続エラー。値タブを開けません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="164"/>
        <source>Cannot reload key value: %1</source>
        <translation>キーの値をリロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="216"/>
        <source>Cannot load key value: %1</source>
        <translation>キーの値をロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="28"/>
        <source>Connect to Redis Server</source>
        <translation>Redisサーバに接続</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="40"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="206"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="114"/>
        <source>Import</source>
        <translation>インポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="41"/>
        <location filename="../../qml/AppToolBar.qml" line="48"/>
        <source>Import Connections</source>
        <translation>接続情報のインポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="58"/>
        <source>Export</source>
        <translation>エクスポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="59"/>
        <location filename="../../qml/AppToolBar.qml" line="66"/>
        <source>Export Connections</source>
        <translation>接続情報のエクスポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="81"/>
        <source>Report issue</source>
        <translation>問題を報告</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="88"/>
        <source>Documentation</source>
        <translation>ドキュメント</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="95"/>
        <source>Join Telegram Chat</source>
        <translation>Telegram Chatに参加</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="102"/>
        <source>Follow</source>
        <translation>フォローする</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="109"/>
        <source>Star on GitHub!</source>
        <translation>GitHubで貢献する!</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="117"/>
        <source>Log</source>
        <translation>ログ</translation>
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
        <translation>新しい接続の設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="144"/>
        <source>How to connect</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="151"/>
        <source>Connection Settings</source>
        <translation>接続の設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="180"/>
        <source>Create connection from Redis URL</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="233"/>
        <source>Learn more about Redis URL:  </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="241"/>
        <source>Connection guides</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="262"/>
        <source>Local or Public Redis</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="278"/>
        <source>Redis with SSL/TLS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="294"/>
        <source>SSH tunnel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="310"/>
        <source>UNIX socket</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="397"/>
        <source>Cannot figure out how to connect to your redis-server?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="403"/>
        <source>&lt;a href=&quot;https://docs.rdm.dev/en/latest/quick-start/&quot;&gt;Read the Docs&lt;/a&gt;, &lt;a href=&quot;mailto:support@rdm.dev&quot;&gt;Contact Support&lt;/a&gt; or ask for help in our &lt;a href=&quot;https://t.me/RedisDesktopManager&quot;&gt;Telegram Group&lt;/a&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="415"/>
        <source>Don&apos;t have running Redis?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="421"/>
        <source>Spin up hassle-free Redis on Digital Ocean</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="433"/>
        <source>Skip</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="468"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="474"/>
        <source>Connection Name</source>
        <translation>接続名</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="480"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="485"/>
        <source>redis-server host</source>
        <translation>Redisサーバのホスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="496"/>
        <source>For better network performance please use 127.0.0.1</source>
        <translation>127.0.0.1を使うとネットワークのパフォーマンスが向上します</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="509"/>
        <source>(Optional) redis-server authentication password</source>
        <translation>(任意) Redisサーバ認証パスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="514"/>
        <source>Username:</source>
        <translation>ユーザー名:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="520"/>
        <source>(Optional) redis-server authentication username (Redis &gt;6.0)</source>
        <translation>(任意) Redisサーバ認証ユーザー名 (Redis &gt;6.0)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="528"/>
        <source>Security</source>
        <translation>セキュリティ</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="559"/>
        <source>Public Key:</source>
        <translation>公開鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="565"/>
        <source>(Optional) Public Key in PEM format</source>
        <translation>(任意) PEM形式の公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="567"/>
        <source>Select public key in PEM format</source>
        <translation>PEM形式の公開鍵を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="578"/>
        <source>(Optional) Private Key in PEM format</source>
        <translation>(任意) PEM形式の非公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="580"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="674"/>
        <source>Select private key in PEM format</source>
        <translation>PEM形式の非公開鍵を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="585"/>
        <source>Authority:</source>
        <translation>証明書:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="591"/>
        <source>(Optional) Authority in PEM format</source>
        <translation>(任意) PEM形式の証明書</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="593"/>
        <source>Select authority file in PEM format</source>
        <translation>PEM形式の証明書を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="612"/>
        <source>SSH Tunnel</source>
        <translation>SSHトンネル</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="631"/>
        <source>SSH Address:</source>
        <translation>SSHアドレス</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="635"/>
        <source>Remote Host with SSH server</source>
        <translation>SSHサーバのリモートホスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="643"/>
        <source>SSH User:</source>
        <translation>SSHユーザー:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="649"/>
        <source>Valid SSH User Name</source>
        <translation>有効なSSHユーザー名</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="572"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="656"/>
        <source>Private Key</source>
        <translation>非公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="672"/>
        <source>Path to Private Key in PEM format</source>
        <translation>PEM形式の非公開鍵のファイルパス</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="682"/>
        <source>&lt;b&gt;Tip:&lt;/b&gt; Use &lt;code&gt;⌘ + Shift + .&lt;/code&gt; to show hidden files and folders in dialog</source>
        <translation>&lt;b&gt;Tip:&lt;/b&gt; &lt;code&gt;⌘ + Shift + .&lt;/code&gt;で隠しファイルや隠しフォルダをダイアログに表示できます</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="687"/>
        <source>Password</source>
        <translation>パスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="698"/>
        <source>SSH User Password</source>
        <translation>SSHユーザーのパスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="709"/>
        <source>Enable TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</source>
        <translation>TLS-over-SSHを有効にする。(&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="156"/>
        <source>Advanced Settings</source>
        <translation>詳細設定</translation>
    </message>
    <message>
        <source>Sign in with rdm.dev account</source>
        <translation type="vanished">rdm.devのアカウントでサインイン</translation>
    </message>
    <message>
        <source>Email:</source>
        <translation type="vanished">Email:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="503"/>
        <source>Password:</source>
        <translation>パスワード:</translation>
    </message>
    <message>
        <source>Application will be restarted to apply this setting.</source>
        <translation type="vanished">設定を有効にするためアプリケーションは再起動されます</translation>
    </message>
    <message>
        <source>Sign In</source>
        <translation type="vanished">サインイン</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="539"/>
        <source>SSL / TLS</source>
        <translation>SSL / TLS</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="598"/>
        <source>Enable strict mode:</source>
        <translation>ストリクトモードを有効にする</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="746"/>
        <source>Keys loading</source>
        <translation>キーの読み込み</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="750"/>
        <source>Default filter:</source>
        <translation>規定のフィルタ:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="756"/>
        <source>Pattern which defines loaded keys from redis-server</source>
        <translation>Redisサーバからロードするキー定義のパターン</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="762"/>
        <source>Namespace Separator:</source>
        <translation>ネームスペースのセパレータ:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="769"/>
        <source>Separator used for namespace extraction from keys</source>
        <translation>キーから抽出するネームスペースに使用するセパレータ</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="775"/>
        <source>Timeouts &amp; Limits</source>
        <translation>タイムアウトと上限</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="779"/>
        <source>Connection Timeout (sec):</source>
        <translation>接続タイムアウト(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="792"/>
        <source>Execution Timeout (sec):</source>
        <translation>実行タイムアウト(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="803"/>
        <source>Databases discovery limit:</source>
        <translation>データベース探索リミット:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="817"/>
        <source>Cluster</source>
        <translation>クラスタ</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="821"/>
        <source>Change host on cluster redirects:</source>
        <translation>クラスタをリダイレクトするホストを変更:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="877"/>
        <source>Invalid settings detected!</source>
        <translation>不正な設定を検出しました!</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="848"/>
        <source>Test Connection</source>
        <translation>接続テスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="885"/>
        <location filename="../../qml/GlobalSettings.qml" line="339"/>
        <location filename="../../qml/common/SaveToFileButton.qml" line="114"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="715"/>
        <source>OK</source>
        <translation>OK</translation>
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
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="47"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="52"/>
        <source>Application will be restarted to apply these settings.</source>
        <translation>アプリケーションを再起動すると設定が有効になります。</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="72"/>
        <source>Language</source>
        <translation>言語</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="84"/>
        <location filename="../../qml/GlobalSettings.qml" line="175"/>
        <source>Font</source>
        <translation>フォント</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="97"/>
        <location filename="../../qml/GlobalSettings.qml" line="188"/>
        <source>Font Size</source>
        <translation>フォントサイズ</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="110"/>
        <location filename="../../qml/GlobalSettings.qml" line="124"/>
        <source>Dark Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="202"/>
        <source>Maximum Formatted Value Size</source>
        <translation>フォーマット済み値の最大サイズ</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="203"/>
        <source>Size in bytes</source>
        <translation>バイト数</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="138"/>
        <source>Use system proxy settings</source>
        <translation>OSのプロキシ設定を使う</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="150"/>
        <source>Use system proxy only for HTTP(S) requests</source>
        <translation>HTTP(S)にシステムのプロキシ設定のみを使う</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="156"/>
        <source>Value Editor</source>
        <translation>値エディタ</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="209"/>
        <source>Connections Tree</source>
        <translation>接続ツリー</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="227"/>
        <source>Show namespaced keys on top</source>
        <translation>ネームスペース付きのキーを上に表示</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="237"/>
        <source>Reopen namespaces on reload</source>
        <translation>リロード時にネームスペースを開きなおす</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="238"/>
        <source>(Disable to improve treeview performance)</source>
        <translation>(無効にするとツリービューが早くなります)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="248"/>
        <source>Show only last part for namespaced keys</source>
        <translation>ネームスペース付きのキーの末尾のみを表示</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="260"/>
        <source>Live update maximum allowed keys</source>
        <translation>ライブアップデートで読み込むキーの最大数</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="272"/>
        <source>Live update interval (in seconds)</source>
        <translation>ライブアップデートの更新頻度(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="282"/>
        <source>External Value View Formatters</source>
        <translation>外部の値ビューフォーマッタ</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="291"/>
        <source>Formatters path: %0</source>
        <translation>フォーマッタのパス: %0</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="308"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="313"/>
        <location filename="../../qml/WelcomeTab.qml" line="30"/>
        <source>Version</source>
        <translation>バージョン</translation>
    </message>
    <message>
        <source>Explore RDM</source>
        <translation type="vanished">RDMの詳細</translation>
    </message>
    <message>
        <source>Before using RDM take a look on the %1</source>
        <translation type="vanished">RDMのご利用前に%1をご覧ください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="857"/>
        <source>Quick Start Guide</source>
        <translation>クイックスタート・ガイド</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="111"/>
        <source>Successful connection to redis-server</source>
        <translation>Redisサーバへの接続に成功</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="114"/>
        <source>Can&apos;t connect to redis-server</source>
        <translation>Redisサーバに接続できません</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="282"/>
        <source>Add Group</source>
        <translation>グループを追加</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="296"/>
        <source>Regroup connections</source>
        <translation>接続グループの編集</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="318"/>
        <source>Exit Regroup Mode</source>
        <translation>接続グループ編集モードを終了</translation>
    </message>
    <message>
        <location filename="../../qml/common/PasswordInput.qml" line="20"/>
        <source>Show password</source>
        <translation>パスワードを表示</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/BetterTreeView.qml" line="214"/>
        <source> (Removed)</source>
        <translation> (削除済)</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="76"/>
        <source>Open Keys Filter</source>
        <translation>キーフィルタを開く</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="80"/>
        <source>Reload Keys in Database</source>
        <translation>データベースのキーをリロード</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="84"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="29"/>
        <source>Add New Key</source>
        <translation>キーを追加</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="90"/>
        <source>Disable Live Update</source>
        <translation>ライブアップデートを無効化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="90"/>
        <source>Enable Live Update</source>
        <translation>ライブアップデートを有効化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="94"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="23"/>
        <source>Open Console</source>
        <translation>コンソールを開く</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="97"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="31"/>
        <source>Analyze Used Memory</source>
        <translation>メモリを分析</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="99"/>
        <source>Bulk Operations</source>
        <translation>バッチ処理</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="120"/>
        <source>Flush Database</source>
        <translation>データベースを初期化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="123"/>
        <source>Delete keys with filter</source>
        <translation>フィルタを用いてキーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="94"/>
        <location filename="../../qml/connections-tree/menu/database.qml" line="126"/>
        <source>Set TTL for multiple keys</source>
        <translation>複数のキーにTTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="129"/>
        <source>Copy keys from this database to another</source>
        <translation>このデータベースから別のデータベースへキーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="132"/>
        <source>Import keys from RDB file</source>
        <translation>RDBファイルからキーをインポート</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="135"/>
        <source>Back</source>
        <translation>戻る</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="21"/>
        <source>Copy Key Name</source>
        <translation>キー名をコピー</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="28"/>
        <source>Reload Namespace</source>
        <translation>ネームスペースをリロード</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="30"/>
        <source>Copy Namespace Pattern</source>
        <translation>ネームスペースのパターンをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="32"/>
        <source>Delete Namespace</source>
        <translation>ネームスペースを削除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="70"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="23"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="13"/>
        <source>Disconnect</source>
        <translation>接続終了</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="19"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="239"/>
        <source>Server Info</source>
        <translation>サーバ情報</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="27"/>
        <source>Reload Server</source>
        <translation>サーバをリロード</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="31"/>
        <source>Unload All Data</source>
        <translation>全てのデータをアンロード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="11"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="35"/>
        <source>Edit Connection Settings</source>
        <translation>接続情報を編集</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="39"/>
        <source>Duplicate Connection</source>
        <translation>接続情報をコピー</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="43"/>
        <source>Delete Connection</source>
        <translation>接続情報を削除</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="21"/>
        <source>Connecting...</source>
        <translation>接続中...</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="193"/>
        <source>Clear</source>
        <translation>消去</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="233"/>
        <source>Arguments</source>
        <translation>パラメーター</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="323"/>
        <location filename="../../qml/console/RedisConsole.qml" line="239"/>
        <source>Description</source>
        <translation>説明</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="245"/>
        <source>Available since</source>
        <translation>利用可能</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="296"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="109"/>
        <source>Redis Version</source>
        <translation>Redisバージョン</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="124"/>
        <source>Used memory</source>
        <translation>消費メモリ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="139"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="247"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="351"/>
        <source>Clients</source>
        <translation>クライアント数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="154"/>
        <source>Commands Processed</source>
        <translation>実行コマンド数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="171"/>
        <source>Uptime</source>
        <translation>稼働時間</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="186"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="488"/>
        <source>Total Keys</source>
        <translation>総キー数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="201"/>
        <source>Hit Ratio</source>
        <translation>ヒット率</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="221"/>
        <source> day(s)</source>
        <translation> 日</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="235"/>
        <source>Info</source>
        <translation>情報</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="299"/>
        <source>Commands Per Second</source>
        <translation>コマンド数/秒</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="313"/>
        <source>Ops/s</source>
        <translation>OP数/秒</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="337"/>
        <source>Connected Clients</source>
        <translation>クライアント接続数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="375"/>
        <source>Memory Usage</source>
        <translation>メモリ消費量</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="387"/>
        <source>Mb</source>
        <translation>MB</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="416"/>
        <source>Network Input</source>
        <translation>ネットワーク入力</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="428"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="464"/>
        <source>Kb/s</source>
        <translation>KB/秒</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="452"/>
        <source>Network Output</source>
        <translation>ネットワーク出力</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="502"/>
        <source>Keys</source>
        <translation>キー</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="585"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="693"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="759"/>
        <source>Auto Refresh</source>
        <translation>自動リフレッシュ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="619"/>
        <source>Property</source>
        <translation>プロパティ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="625"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="22"/>
        <source>Value</source>
        <translation>値</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="847"/>
        <source>Subscribe in Console</source>
        <translation>コンソール上でサブスクライブ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="243"/>
        <source>Slowlog</source>
        <translation>Slowlog</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="252"/>
        <source>Pub/Sub Channels</source>
        <translation>Pub/Subチャンネル</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="817"/>
        <source>Enable</source>
        <translation>有効</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="836"/>
        <source>Channel Name</source>
        <translation>チャンネル名</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="317"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="708"/>
        <source>Command</source>
        <translation>コマンド</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="725"/>
        <source>Processed at</source>
        <translation>処理</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="740"/>
        <source>Execution Time (μs)</source>
        <translation>実行時間(μs)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="774"/>
        <source>Client Address</source>
        <translation>クライアントのアドレス</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="780"/>
        <source>Age (sec)</source>
        <translation>経過時間(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="786"/>
        <source>Idle</source>
        <translation>アイドル</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="792"/>
        <source>Flags</source>
        <translation>フラグ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="798"/>
        <source>Current Database</source>
        <translation>現在のデータベース</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="9"/>
        <source>Add New Key to </source>
        <translation>新しいキーを追加 </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="25"/>
        <location filename="../../qml/value-editor/editors/HashItemEditor.qml" line="17"/>
        <source>Key:</source>
        <translation>キー:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="35"/>
        <source>Type:</source>
        <translation>型:</translation>
    </message>
    <message>
        <location filename="../../qml/common/BetterDialog.qml" line="39"/>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="47"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="71"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="255"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="437"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="11"/>
        <source>Edit Connections Group</source>
        <translation>接続グループを編集</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="11"/>
        <source>Add New Connections Group</source>
        <translation>接続グループの新規追加</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="30"/>
        <source>Group Name:</source>
        <translation>グループ名:</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/ConnectionGroupDialog.qml" line="71"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="118"/>
        <location filename="../../qml/value-editor/editors/formatters/ValueFormatters.qml" line="219"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="12"/>
        <source>Page</source>
        <translation>ページ</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="54"/>
        <source>Enter valid value</source>
        <translation>有効な値を入力</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="264"/>
        <source>Formatting error</source>
        <translation>フォーマットエラー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="269"/>
        <source>Unknown formatter error (Empty response)</source>
        <translation>予期せぬフォーマッタのエラー&#x3000;(空の応答)</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="297"/>
        <source>[Binary]</source>
        <translation>[バイナリ]</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="300"/>
        <source> [Compressed: </source>
        <translation> [圧縮: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="385"/>
        <source>Copy to Clipboard</source>
        <translation>クリップボードにコピー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="438"/>
        <source>Save Changes</source>
        <translation>変更を保存</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="542"/>
        <source>Search string</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="556"/>
        <source>Find Next</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="556"/>
        <source>Find</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="588"/>
        <source>Regex</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="574"/>
        <source>Cannot find more results</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="575"/>
        <source>Cannot find any results</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="711"/>
        <source>Binary value is too large to display</source>
        <translation>バイナリが大きすぎるため表示できません</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="306"/>
        <source>View as:</source>
        <translation>表示形式:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="343"/>
        <source>Large value (&gt;150kB). Formatters are not available.</source>
        <translation>値が大きすぎます(&gt;150kB)。フォーマッタは使用できません。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="18"/>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="30"/>
        <source>Score</source>
        <translation>ソース</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="10"/>
        <source>Bulk Operations Manager</source>
        <translation type="unfinished">バッチ処理マネージャ</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="63"/>
        <source>Invalid RDB path</source>
        <translation type="unfinished">不正なRDBのパスです</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="63"/>
        <source>Please specify valid path to RDB file</source>
        <translation type="unfinished">RDBファイルへの正しいパスを指定してください</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="85"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="86"/>
        <source>Delete keys</source>
        <translation type="unfinished">キーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="95"/>
        <source>Set TTL</source>
        <translation type="unfinished">TTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="103"/>
        <source>Copy keys to another database</source>
        <translation type="unfinished">他のデータベースにキーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="104"/>
        <source>Copy keys</source>
        <translation type="unfinished">キーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="113"/>
        <source>Import data from rdb file</source>
        <translation type="unfinished">RDBファイルからデータをインポート</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="148"/>
        <source>Redis Server:</source>
        <translation type="unfinished">Redisサーバ:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="160"/>
        <source>Database number:</source>
        <translation type="unfinished">データベース番号:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="180"/>
        <source>Path to RDB file:</source>
        <translation type="unfinished">RDBファイルへのパス:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="191"/>
        <source>Path to dump.rdb file</source>
        <translation>dump.rdbファイルへのパス</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="193"/>
        <source>Select dump.rdb</source>
        <translation>dump.rdbを選択</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="202"/>
        <source>Select DB in RDB file:</source>
        <translation type="unfinished">RDBファイルからDBを選択</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="222"/>
        <source>Import keys that match &lt;b&gt;regex&lt;/b&gt;:</source>
        <translation type="unfinished">&lt;b&gt;正規表現&lt;/b&gt;に一致するキーをインポート:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="222"/>
        <source>Key pattern:</source>
        <translation type="unfinished">キーパターン:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="269"/>
        <source>Destination Redis Server:</source>
        <translation type="unfinished">Redisサーバの接続先:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="280"/>
        <source>Destination Redis Server Database Index:</source>
        <translation type="unfinished">Redisデータベースインデックスの接続先:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="317"/>
        <source>Show matched keys</source>
        <translation type="unfinished">一致するキーを表示</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="317"/>
        <source>Show Affected keys</source>
        <translation type="unfinished">影響するキーを表示</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="340"/>
        <source>Matched keys:</source>
        <translation type="unfinished">一致したキー:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="340"/>
        <source>Affected keys:</source>
        <translation type="unfinished">影響するキー:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="365"/>
        <source>Bulk Operation finished.</source>
        <translation type="unfinished">バッチ処理が完了しました。</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="370"/>
        <source>Bulk Operation finished with errors</source>
        <translation type="unfinished">バッチ処理でエラーが発生しました</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="422"/>
        <source>Processed: </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="424"/>
        <source>Getting list of affected keys...</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="489"/>
        <source>Confirmation</source>
        <translation type="unfinished">確認</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="490"/>
        <source>Do you really want to perform bulk operation?</source>
        <translation type="unfinished">バッチ処理を本当に実行しますか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="18"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="61"/>
        <source>Value (represented as JSON object)</source>
        <translation>値(JSONオブジェクト形式)</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/listkey.cpp" line="127"/>
        <source>The row has been changed on server.Reload and try again.</source>
        <translation>サーバ上のROWが更新されました。リロードしてからやりなおしてください。</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/bulkoperationsmanager.cpp" line="122"/>
        <source>Failed to perform actions on %1 keys. </source>
        <translation>%1キーに対する処理が失敗しました</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="12"/>
        <source>Cannot copy key </source>
        <translation>キーをコピーできません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="119"/>
        <source>Source connection error</source>
        <translation>ソース接続エラー</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="131"/>
        <source>Target connection error</source>
        <translation>ターゲット接続エラー</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/deleteoperation.cpp" line="11"/>
        <source>Cannot remove key </source>
        <translation>キーを削除できません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="16"/>
        <source>Cannot execute command </source>
        <translation>コマンドを実行できません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/ttloperation.cpp" line="10"/>
        <source>Cannot set TTL for key </source>
        <translation>TTLをキーに設定できません</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/abstractnamespaceitem.cpp" line="157"/>
        <source>Your redis-server doesn&apos;t support &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; commands.</source>
        <translation>あなたのRedisサーバは&lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt;コマンドをサポートしていません。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="136"/>
        <source>Key was added. Do you want to reload keys in selected namespace?</source>
        <translation>キーを追加しました。選択されているネームスペースのキーをリロードしますか?</translation>
    </message>
    <message>
        <location filename="../../qml/common/FilePathInput.qml" line="27"/>
        <source>Select File</source>
        <translation>ファイルを選択</translation>
    </message>
    <message>
        <source>Save to File</source>
        <translation type="vanished">ファイルに保存</translation>
    </message>
    <message>
        <source>Save Value</source>
        <translation type="vanished">値を保存</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="723"/>
        <source>Save value to file</source>
        <translation>値をファイルに保存</translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="9"/>
        <source>Save Raw Value to File</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="9"/>
        <source>Save Formatted Value to File</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="26"/>
        <source>Save Raw Value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="26"/>
        <source>Save Formatted Value</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="50"/>
        <source>Save raw value to file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="50"/>
        <source>Save formatted value to file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/common/SaveToFileButton.qml" line="71"/>
        <source>Value was saved to file:</source>
        <translation>値をファイルに保存しました:</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/abstractoperation.cpp" line="34"/>
        <source>Cannot connect to redis-server</source>
        <translation>Redisサーバに接続できません</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="12"/>
        <source>Edit Connection Group</source>
        <translation>接続グループの編集</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server_group.qml" line="16"/>
        <source>Delete Connection Group</source>
        <translation>接続グループの削除</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/servergroup.cpp" line="58"/>
        <source>Do you really want to delete group &lt;b&gt;with all connections&lt;/b&gt;?</source>
        <translation>グループを本当に削除しますか？&#x3000;&lt;b&gt;グループの接続情報もすべて失われます&lt;/b&gt;</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="8"/>
        <source>Order of elements:</source>
        <translation>要素の順序:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="20"/>
        <source>Default</source>
        <translation>デフォルト</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/ListFilters.qml" line="21"/>
        <source>Reverse</source>
        <translation>降順</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="28"/>
        <source>Start date should be less than End date</source>
        <translation>開始日は終了日より前でなければなりません</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/filters/StreamFilters.qml" line="134"/>
        <source>Apply filter</source>
        <translation>フィルタを適用</translation>
    </message>
    <message>
        <source>Download was canceled</source>
        <translation type="obsolete">ダウンロードをキャンセルしました</translation>
    </message>
    <message>
        <source>Network error</source>
        <translation type="obsolete">ネットワークエラー</translation>
    </message>
    <message>
        <location filename="../../qml/WelcomeTab.qml" line="31"/>
        <source>&lt;span style=&quot;font-size: 11px;&quot;&gt;Powered by awesome &lt;a href=&quot;https://github.com/uglide/RedisDesktopManager/tree/2021/3rdparty&quot;&gt;open-source software&lt;/a&gt; and &lt;a href=&quot;http://icons8.com/&quot;&gt;icons8&lt;/a&gt;.&lt;/span&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="11"/>
        <source>Getting Started</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="41"/>
        <source>Thank you for choosing RDM. Let&apos;s make your Redis experience better.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="59"/>
        <source>Connect to Redis-Server</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="72"/>
        <source>Read the Docs</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
