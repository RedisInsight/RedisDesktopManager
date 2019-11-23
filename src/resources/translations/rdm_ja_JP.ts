<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>BulkOperationsDialog</name>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="10"/>
        <source>Bulk Operations Manager</source>
        <translation>バッチ処理マネージャ</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="59"/>
        <source>Invalid RDB path</source>
        <translation>不正なRDBのパスです</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="59"/>
        <source>Please specify valid path to RDB file</source>
        <translation>RDBファイルへの正しいパスを指定してください</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="75"/>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="76"/>
        <source>Delete keys</source>
        <translation>キーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="85"/>
        <source>Set TTL for multiple keys</source>
        <translation>複数のキーのTTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="86"/>
        <source>Set TTL</source>
        <translation>TTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="95"/>
        <source>Copy keys to another database</source>
        <translation>他のデータベースにキーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="96"/>
        <source>Copy keys</source>
        <translation>キーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="106"/>
        <source>Import data from rdb file</source>
        <translation>RDBファイルからデータをインポート</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="107"/>
        <source>Import</source>
        <translation>インポート</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="141"/>
        <source>Redis Server:</source>
        <translation>Redisサーバ:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="151"/>
        <source>Database number:</source>
        <translation>データベース番号:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="169"/>
        <source>Path to RDB file:</source>
        <translation>RDBファイルへのパス:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="191"/>
        <source>Select DB in RDB file:</source>
        <translation>RDBファイルからDBを選択</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="212"/>
        <source>Import keys that match &lt;b&gt;regex&lt;/b&gt;:</source>
        <translation>&lt;b&gt;正規表現&lt;/b&gt;に一致するキーをインポート:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="212"/>
        <source>Key pattern:</source>
        <translation>キーパターン:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="258"/>
        <source>Destination Redis Server:</source>
        <translation>Redisサーバの接続先:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="268"/>
        <source>Destination Redis Server Database Index:</source>
        <translation>Redisデータベースインデックスの接続先:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="308"/>
        <source>Show matched keys</source>
        <translation>一致するキーを表示</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="308"/>
        <source>Show Affected keys</source>
        <translation>影響するキーを表示</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="331"/>
        <source>Matched keys:</source>
        <translation>一致したキー:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="331"/>
        <source>Affected keys:</source>
        <translation>影響するキー:</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="370"/>
        <source>Bulk Operation finished.</source>
        <translation>バッチ処理が完了しました。</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="375"/>
        <source>Bulk Operation finished with errors</source>
        <translation>バッチ処理でエラーが発生しました</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="404"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="470"/>
        <source>Confirmation</source>
        <translation>確認</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="471"/>
        <source>Do you really want to perform bulk operation?</source>
        <translation>バッチ処理を本当に実行しますか?</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="260"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="292"/>
        <source>Cannot connect to cluster node %1:%2</source>
        <translation>クラスタノードに書き込みできません　%1:%2</translation>
    </message>
    <message>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="303"/>
        <location filename="../../../3rdparty/qredisclient/src/qredisclient/connection.cpp" line="321"/>
        <source>Cannot flush db (%1): %2</source>
        <translation>DBをフラッシュできません　(%1): %2</translation>
    </message>
</context>
<context>
    <name>RDM</name>
    <message>
        <location filename="../../app/app.cpp" line="57"/>
        <source>Settings directory is not writable</source>
        <translation>設定したディレクトリは書き込みできません</translation>
    </message>
    <message>
        <location filename="../../app/app.cpp" line="59"/>
        <source>RDM can&apos;t save connections file to settings directory. Please change file permissions or restart RDM as administrator.</source>
        <translation>RDMは設定ディレクトリに接続情報を保存できませんでした。ファイルのパーミッションを変更するか、RDMを管理者として再起動してください。</translation>
    </message>
    <message>
        <location filename="../../app/app.cpp" line="306"/>
        <source>Please download new version of Redis Desktop Manager: %1</source>
        <translation>新バージョンのRedis Desktop Managerをダウンロードしてください: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="67"/>
        <source>Key with new name %1 already exist in database or original key was removed</source>
        <translation>新しいキー%1はすでにデータベースに存在しているか、または元のキーが削除されました</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="123"/>
        <source>Cannot parse scan response</source>
        <translation>スキャンのレスポンスをパースできません</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="283"/>
        <source>Server returned unexpected response: </source>
        <translation>サーバが不正なレスポンスを返しました:</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="88"/>
        <source>Cannot set TTL for key %1</source>
        <translation>キー%1にTTLを設定できません</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/abstractkey.h" line="229"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="240"/>
        <source>Cannot load rows for key %1: %2</source>
        <translation>キー%1のROWを読むことができません: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="42"/>
        <location filename="../../app/models/key-models/hashkey.cpp" line="75"/>
        <location filename="../../app/models/key-models/listkey.cpp" line="14"/>
        <location filename="../../app/models/key-models/listkey.cpp" line="35"/>
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
        <location filename="../../app/models/key-models/abstractkey.h" line="147"/>
        <location filename="../../app/models/key-models/abstractkey.h" line="295"/>
        <location filename="../../app/models/treeoperations.cpp" line="52"/>
        <location filename="../../app/models/treeoperations.cpp" line="101"/>
        <location filename="../../modules/console/consolemodel.cpp" line="65"/>
        <source>Connection error: </source>
        <translation>接続エラー: </translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="113"/>
        <source>Value with the same key already exist</source>
        <translation>同じキーを持つ値がすでに存在しています</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/hashkey.cpp" line="136"/>
        <location filename="../../app/models/key-models/sortedsetkey.cpp" line="136"/>
        <source>Data was loaded from server partially.</source>
        <translation>サーバーからデータが部分的にロードされました。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="27"/>
        <source>Cannot load key %1, connection error occurred: %2</source>
        <translation>キー%1を読めません。接続エラーが発生しました: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="38"/>
        <source>Cannot load key %1 because it doesn&apos;t exist in database. Please reload connection tree and try again.</source>
        <translation>データベースに存在しないためキー%1を読めません。接続ツリーをリロードしてから改めて試してください。</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="65"/>
        <source>Cannot load TTL for key %1, connection error occurred: %2</source>
        <translation>キー%1のTTLがロードできません。接続エラーが発生しました: %2</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="57"/>
        <source>Unsupported Redis Data type %1</source>
        <translation>サポートしていないRedisのデータ型です　%1</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/keyfactory.cpp" line="81"/>
        <source>Cannot retrive type of the key: </source>
        <translation>キーの型を取得できません: </translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="81"/>
        <source>Cannot connect to server &apos;%1&apos;. Check log for details.</source>
        <translation>サーバー&apos;%1&apos;に接続できません。詳細はログを確認してください。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="89"/>
        <source>Open Source version of Redis Desktop Manager &lt;b&gt;doesn&apos;t support SSH tunneling&lt;/b&gt;.&lt;br /&gt;&lt;br /&gt; To get fully-featured application, please buy subscription on &lt;a href=&apos;https://redisdesktop.com/subscriptions&apos;&gt;redisdesktop.com&lt;/a&gt;. &lt;br/&gt;&lt;br /&gt;Every single subscription gives us funds to continue the development process and provide support to our users. &lt;br /&gt;If you have any questions please feel free to contact us at &lt;a href=&apos;mailto:support@redisdesktop.com&apos;&gt;support@redisdesktop.com&lt;/a&gt; or join &lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;.</source>
        <translation>オープンソース版のRedis Desktop ManagerはSSHトンネルをサポートして&lt;b&gt;いません&lt;/b&gt;。&lt;br /&gt;&lt;br /&gt;全ての機能を有効にするには&lt;a href=&apos;https://redisdesktop.com/subscriptions&apos;&gt;redisdesktop.com&lt;/a&gt;でサブスクリプションに加入してください。&lt;br/&gt;&lt;br /&gt;ご加入の皆様からいただきました資金により、私たちは開発を続けることができ、皆様をサポートできます。&lt;br /&gt;もしご質問がございましたらお気軽に&lt;a href=&apos;mailto:support@redisdesktop.com&apos;&gt;support@redisdesktop.com&lt;/a&gt;または&lt;a href=&apos;https://t.me/RedisDesktopManager&apos;&gt;Telegram chat&lt;/a&gt;までお問い合わせください。</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="182"/>
        <source>Cannot load keys: %1</source>
        <translation>キーをロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="240"/>
        <source>Delete key error: %1</source>
        <translation>キー削除エラー: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="314"/>
        <source>Cannot used memory for key: %1</source>
        <translation>キーのメモリを使えませんでした: %1</translation>
    </message>
    <message>
        <location filename="../../app/models/treeoperations.cpp" line="287"/>
        <source>Cannot flush database: </source>
        <translation>データベースをフラッシュできません: </translation>
    </message>
    <message>
        <location filename="../../modules/common/tabmodel.cpp" line="39"/>
        <source>Invalid Connection. Check connection settings.</source>
        <translation>不正な接続です。接続の設定を確認してください。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="217"/>
        <source>Key was added. Do you want to reload keys in selected database?</source>
        <translation>キーを追加しました。選択したデータベースのキーをリロードしますか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="225"/>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="102"/>
        <source>Key was added</source>
        <translation>キーを追加しました。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="233"/>
        <source>Another operation is currently in progress</source>
        <translation>別の処理が動作しています。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="235"/>
        <source>Please wait until another operation will be finised.</source>
        <translation>処理が終わるまでお待ちください。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="246"/>
        <source>Do you really want to remove all keys from this database?</source>
        <translation>このデータベースから全てのキーを削除しても本当によろしいですか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="78"/>
        <source>Cannot load databases:

</source>
        <translation>データベースをロードできません:

</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="179"/>
        <source>Live update was disabled</source>
        <translation>ライブ・アップデートは無効です</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/databaseitem.cpp" line="180"/>
        <source>Live update was disabled due to exceeded keys limit. Please specify filter more carrfully or change limit in settings.</source>
        <translation>キーの上限を超えているためライブアップデートは無効です。より適切なフィルタを指定するか、または設定で上限を変更してください。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="134"/>
        <source>Rename</source>
        <translation>名前の変更</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="138"/>
        <source>Rename key</source>
        <translation>キー名の変更</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="147"/>
        <source>New name:</source>
        <translation>新しい名前:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="177"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="257"/>
        <source>Size: </source>
        <translation>サイズ: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="182"/>
        <source>TTL:</source>
        <translation>TTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="187"/>
        <source>Set key TTL</source>
        <translation>キーのTTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="196"/>
        <source>New TTL:</source>
        <translation>新しいTTL:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="227"/>
        <source>Delete</source>
        <translation>削除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/key.qml" line="22"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="232"/>
        <source>Delete key</source>
        <translation>キーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="233"/>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="137"/>
        <source>Do you really want to delete this key?</source>
        <translation>このキーを本当に削除してもよろしいですか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="249"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="560"/>
        <source>Reload Value</source>
        <translation>値をリロード</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="443"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="452"/>
        <source>Add Row</source>
        <translation>ROWを追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="451"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="700"/>
        <source>Add Element to HLL</source>
        <translation>HLLに要素を追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="492"/>
        <source>Add</source>
        <translation>追加</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="523"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="543"/>
        <source>Delete row</source>
        <translation>ROWを削除</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="529"/>
        <source>The row is the last one in the key. After removing it key will be deleted.</source>
        <translation>このROWはこのキーの最後の1つです。削除後はキーも削除されます。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="531"/>
        <source>Do you really want to remove this row?</source>
        <translation>このROWを本当に削除しますか?</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="591"/>
        <source>Search on page...</source>
        <translation>ページを検索...</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="618"/>
        <source>Search through All values</source>
        <translation>すべての値を検索</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="714"/>
        <source>Nothing to save</source>
        <translation>保存できるデータがありません</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="726"/>
        <source>Value was updated!</source>
        <translation>値を更新しました!</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="734"/>
        <source>Save value</source>
        <translation>値を保存</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/keyitem.cpp" line="146"/>
        <source>Key error</source>
        <translation>キーエラー</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="153"/>
        <source>Value and Console tabs related to this connection will be closed. Do you want to continue?</source>
        <translation>この接続に関連するValueとConsoleのタブを閉じます。続行しますか?</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/serveritem.cpp" line="165"/>
        <source>Do you really want to delete connection?</source>
        <translation>接続を本当に削除しますか?</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="12"/>
        <source>Connected to cluster.
</source>
        <translation>クラスタに接続しました。
</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="15"/>
        <source>Connected.
</source>
        <translation>接続しました。
</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="41"/>
        <source>Switch to Pub/Sub mode. Close console tab to stop listen for messages.</source>
        <translation>Pub/Subモードに変更する。メッセージを見るのをやめるにはConsoleのタブを閉じてください。</translation>
    </message>
    <message>
        <location filename="../../modules/console/consolemodel.cpp" line="50"/>
        <source>Subscribe error: %1</source>
        <translation>サブスクライブエラー: %1</translation>
    </message>
    <message>
        <location filename="../../modules/server-stats/serverstatsmodel.cpp" line="94"/>
        <source>Server %0</source>
        <translation>サーバー %0</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="171"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="207"/>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="236"/>
        <source>Can&apos;t find formatter with name: %1</source>
        <translation>指定したフォーマッタが見つかりません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="177"/>
        <source>Invalid callback</source>
        <translation>不正なコールバックです</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="191"/>
        <source>Cannot decode value using %1 formatter. </source>
        <translation>フォーマッタ%1で値をデコードすることができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="221"/>
        <source>Cannot validate value using %1 formatter.</source>
        <translation>フォーマッタ%1で値を検証することができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/externalformattersmanager.cpp" line="250"/>
        <source>Cannot encode value using %1 formatter. </source>
        <translation>フォーマッタ%1で値をエンコードすることができません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="22"/>
        <source>Cannot open value tab</source>
        <translation>Valueタブを開くことができません</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/tabsmodel.cpp" line="35"/>
        <source>Connection error. Can&apos;t open value tab. </source>
        <translation>接続エラー。Valueタブを開けません。</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="93"/>
        <source>Cannot reload key value: %1</source>
        <translation>キーの値をリロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/value-editor/valueviewmodel.cpp" line="136"/>
        <source>Cannot load key value: %1</source>
        <translation>キーの値をロードできません: %1</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="14"/>
        <source>Connect to Redis Server</source>
        <translation>Redisサーバに接続</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="26"/>
        <location filename="../../qml/AppToolBar.qml" line="33"/>
        <location filename="../../qml/AppToolBar.qml" line="49"/>
        <source>Import Connections</source>
        <translation>接続情報のインポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="42"/>
        <source>Export Connections</source>
        <translation>接続情報のエクスポート</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="62"/>
        <location filename="../../qml/AppToolBar.qml" line="63"/>
        <source>Report issue</source>
        <translation>問題を報告</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="69"/>
        <location filename="../../qml/AppToolBar.qml" line="70"/>
        <source>Documentation</source>
        <translation>ドキュメント</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="76"/>
        <location filename="../../qml/AppToolBar.qml" line="77"/>
        <source>Join Telegram Chat</source>
        <translation>Telegram Chatに参加</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="83"/>
        <location filename="../../qml/AppToolBar.qml" line="84"/>
        <source>Follow</source>
        <translation>フォローする</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="90"/>
        <location filename="../../qml/AppToolBar.qml" line="91"/>
        <source>Star on GitHub!</source>
        <translation>GitHubで貢献する!</translation>
    </message>
    <message>
        <location filename="../../qml/AppToolBar.qml" line="99"/>
        <location filename="../../qml/GlobalSettings.qml" line="13"/>
        <source>Settings</source>
        <translation>設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="12"/>
        <source>New Connection Settings</source>
        <translation>新しい接続の設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="138"/>
        <source>Connection Settings</source>
        <translation>接続の設定</translation>
    </message>
    <message>
        <source>Main Settings</source>
        <translation type="vanished">メインの設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="156"/>
        <source>Name:</source>
        <translation>名前:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="162"/>
        <source>Connection Name</source>
        <translation>接続名</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="168"/>
        <source>Address:</source>
        <translation>アドレス:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="172"/>
        <source>redis-server host</source>
        <translation>Redisサーバーのホスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="180"/>
        <source>Auth:</source>
        <translation>認証:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="185"/>
        <source>(Optional) redis-server authentication password</source>
        <translation>(任意) Redisサーバ認証パスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="191"/>
        <source>Security</source>
        <translation>セキュリティ</translation>
    </message>
    <message>
        <source>None</source>
        <translation type="vanished">なし</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="200"/>
        <source>SSL</source>
        <translation>SSL</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="223"/>
        <source>Public Key:</source>
        <translation>公開鍵:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="228"/>
        <source>(Optional) Public Key in PEM format</source>
        <translation>(任意) PEM形式の公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="230"/>
        <source>Select public key in PEM format</source>
        <translation>PEM形式の公開鍵を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="240"/>
        <source>(Optional) Private Key in PEM format</source>
        <translation>(任意) PEM形式の非公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="242"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="331"/>
        <source>Select private key in PEM format</source>
        <translation>PEM形式の非公開鍵を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="247"/>
        <source>Authority:</source>
        <translation>証明書:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="252"/>
        <source>(Optional) Authority in PEM format</source>
        <translation>(任意) PEM形式の証明書</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="254"/>
        <source>Select authority file in PEM format</source>
        <translation>PEM形式の証明書を選択してください</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="264"/>
        <source>SSH Tunnel</source>
        <translation>SSHトンネル</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="288"/>
        <source>SSH Address:</source>
        <translation>SSHアドレス</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="292"/>
        <source>Remote Host with SSH server</source>
        <translation>SSHサーバのリモートホスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="300"/>
        <source>SSH User:</source>
        <translation>SSHユーザー:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="306"/>
        <source>Valid SSH User Name</source>
        <translation>正しいSSHユーザー名</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="235"/>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="313"/>
        <source>Private Key</source>
        <translation>非公開鍵</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="329"/>
        <source>Path to Private Key in PEM format</source>
        <translation>PEM形式の非公開鍵のファイルパス</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="339"/>
        <source>&lt;b&gt;Tip:&lt;/b&gt; Use &lt;code&gt;⌘ + Shift + .&lt;/code&gt; to show hidden files and folders in dialog</source>
        <translation>&lt;b&gt;Tip:&lt;/b&gt; &lt;code&gt;⌘ + Shift + .&lt;/code&gt;で隠しファイルや隠しフォルダをダイアログに表示できます</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="344"/>
        <source>Password</source>
        <translation>パスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="355"/>
        <source>SSH User Password</source>
        <translation>SSHユーザーのパスワード</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="366"/>
        <source>Enable TLS-over-SSH (&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</source>
        <translation>TLS-over-SSHを有効にする。(&lt;b&gt;AWS ElastiCache&lt;/b&gt; &lt;b&gt;Encryption in-transit&lt;/b&gt;)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="378"/>
        <source>Advanced Settings</source>
        <translation>詳細設定</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="387"/>
        <source>Keys loading</source>
        <translation>キーの読み込み</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="391"/>
        <source>Default filter:</source>
        <translation>規定のフィルタ:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="397"/>
        <source>Pattern which defines loaded keys from redis-server</source>
        <translation>Redisサーバーからロードしたキーを定義するパターン</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="403"/>
        <source>Namespace Separator:</source>
        <translation>ネームスペースのセパレータ:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="410"/>
        <source>Separator used for namespace extraction from keys</source>
        <translation>キーから抽出するネームスペースに使用するセパレータ</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="415"/>
        <source>Use server-side optimized keys loading (experimental):</source>
        <translation>サーバー側のキー読み込み最適化機能を使用する(テスト版)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="426"/>
        <source>Timeouts &amp; Limits</source>
        <translation>タイムアウトと上限</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="430"/>
        <source>Connection Timeout (sec):</source>
        <translation>接続タイムアウト(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="443"/>
        <source>Execution Timeout (sec):</source>
        <translation>実行タイムアウト(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="454"/>
        <source>Databases discovery limit:</source>
        <translation>データベース探索リミット:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="468"/>
        <source>Cluster</source>
        <translation>クラスタ</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="472"/>
        <source>Change host on cluster redirects:</source>
        <translation>クラスタをリダイレクトするホストを変更:</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="482"/>
        <source>TLS</source>
        <translation>TLS</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="486"/>
        <source>Ignore all SSL/TLS errors (&lt;b&gt;dangerous&lt;/b&gt;):</source>
        <translation>すべてのSSL/TLSエラーを無視する(&lt;b&gt;検証用&lt;/b&gt;):</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="514"/>
        <source>Invalid settings detected!</source>
        <translation>不正な設定を検出しました!</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="524"/>
        <source>Test Connection</source>
        <translation>接続テスト</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="544"/>
        <location filename="../../qml/GlobalSettings.qml" line="202"/>
        <location filename="../../qml/QuickStartDialog.qml" line="54"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="556"/>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="103"/>
        <source>Cancel</source>
        <translation>キャンセル</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="41"/>
        <source>General</source>
        <translation>一般</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="52"/>
        <source>Language</source>
        <translation>言語</translation>
    </message>
    <message>
        <source>Application restart is needed to apply this setting.</source>
        <translation type="vanished">設定を適用するにはアプリを再起動してください。</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="53"/>
        <location filename="../../qml/GlobalSettings.qml" line="67"/>
        <location filename="../../qml/GlobalSettings.qml" line="81"/>
        <location filename="../../qml/GlobalSettings.qml" line="94"/>
        <source>Application will be restarted to apply this setting.</source>
        <translation>設定を有効にするためアプリケーションは再起動されます。</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="66"/>
        <source>Font</source>
        <translation>フォント</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="80"/>
        <source>Font Size</source>
        <translation>フォントサイズ</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="93"/>
        <source>Use system proxy settings</source>
        <translation>OSのプロキシ設定を使う</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="100"/>
        <source>Connections Tree</source>
        <translation>接続ツリー</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="111"/>
        <source>Reopen namespaces on reload</source>
        <translation>リロード時にネームスペースを開きなおす</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="112"/>
        <location filename="../../qml/GlobalSettings.qml" line="123"/>
        <source>(Disable to improve treeview performance)</source>
        <translation>(無効にするとツリービューが早くなります)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="122"/>
        <source>Enable key sorting in tree</source>
        <translation>ツリーのキーをソートする</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="135"/>
        <source>Live update maximum allowed keys</source>
        <translation>ライブアップデートが許容するキーの最大数</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="148"/>
        <source>Live update interval (in seconds)</source>
        <translation>ライブアップデートの更新頻度(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="154"/>
        <source>外部のValueビューフォーマッタ</source>
        <translation></translation>
    </message>
    <message>
        <source>Custom Value View Formatters</source>
        <translation type="vanished">Valueビューのカスタムフォーマット</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="159"/>
        <source>Formatters path: %0</source>
        <translation>フォーマッタのパス: %0</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="172"/>
        <source>Name</source>
        <translation>名称</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="177"/>
        <source>Version</source>
        <translation>バージョン</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="11"/>
        <source>Explore Redis Desktop Manager</source>
        <translation>Redis Desktop Managerの詳細</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="37"/>
        <source>Before using Redis Desktop Manager (RDM) take a look on the %1</source>
        <translation>Redis Desktop Manager(RDM)のご利用前に%1をご覧ください</translation>
    </message>
    <message>
        <location filename="../../qml/QuickStartDialog.qml" line="38"/>
        <source>Quick Start Guide</source>
        <translation>クイックスタート・ガイド</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="85"/>
        <source>Successful connection to redis-server</source>
        <translation>Redisサーバへの接続に成功</translation>
    </message>
    <message>
        <location filename="../../qml/app.qml" line="88"/>
        <source>Can&apos;t connect to redis-server</source>
        <translation>Redisサーバに接続できません</translation>
    </message>
    <message>
        <source>Sign in with RedisDesktop.com account</source>
        <translation type="vanished">RedisDesktop.comのアカウントでサインイン</translation>
    </message>
    <message>
        <source>Email:</source>
        <translation type="vanished">Email:</translation>
    </message>
    <message>
        <source>Password:</source>
        <translation type="vanished">パスワード</translation>
    </message>
    <message>
        <location filename="../../qml/common/PasswordInput.qml" line="20"/>
        <source>Show password</source>
        <translation>パスワードを表示</translation>
    </message>
    <message>
        <source>Sign In</source>
        <translation type="vanished">サインイン</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/BetterTreeView.qml" line="91"/>
        <source> (Removed)</source>
        <translation> (削除済)</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="74"/>
        <source>Open Keys Filter</source>
        <translation>キーフィルタを開く</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="78"/>
        <source>Reload Keys in Database</source>
        <translation>データベースのキーをリロード</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="82"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="29"/>
        <source>Add New Key</source>
        <translation>キーを追加</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="88"/>
        <source>Disable Live Update</source>
        <translation>ライブアップデートを無効化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="88"/>
        <source>Enable Live Update</source>
        <translation>ライブアップデートを有効化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="92"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="23"/>
        <source>Open Console</source>
        <translation>コンソールを開く</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="95"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="31"/>
        <source>Analyze Used Memory</source>
        <translation>メモリを分析</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="97"/>
        <source>Bulk Operations</source>
        <translation>バッチ処理</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="118"/>
        <source>Flush Database</source>
        <translation>データベースを初期化</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="121"/>
        <source>Delete keys with filter</source>
        <translation>フィルタを用いてキーを削除</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="124"/>
        <source>Set TTL for multiple keys</source>
        <translation>複数のキーにTTLを設定</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="127"/>
        <source>Copy keys from this database to another</source>
        <translation>このデータベースから別のデータベースへキーをコピー</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="130"/>
        <source>Import keys from RDB file</source>
        <translation>RDBファイルからキーをインポート</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="133"/>
        <source>Back</source>
        <translation>戻る</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/database.qml" line="153"/>
        <source>Enter Filter</source>
        <translation>フィルターを入力</translation>
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
        <location filename="../../qml/connections-tree/menu/database.qml" line="68"/>
        <location filename="../../qml/connections-tree/menu/namespace.qml" line="23"/>
        <location filename="../../qml/connections-tree/menu/server.qml" line="13"/>
        <source>Disconnect</source>
        <translation>接続終了</translation>
    </message>
    <message>
        <location filename="../../qml/connections-tree/menu/server.qml" line="19"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="234"/>
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
        <location filename="../../qml/ConnectionSettignsDialog.qml" line="12"/>
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
        <location filename="../../qml/console/Consoles.qml" line="66"/>
        <source>Confirm Action</source>
        <translation>アクションを確認</translation>
    </message>
    <message>
        <location filename="../../qml/console/Consoles.qml" line="75"/>
        <source>Do you really want to close console with running command?</source>
        <translation>コマンド実行中ですがConsoleを本当に閉じますか?</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="20"/>
        <source>Connecting...</source>
        <translation>接続中...</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="192"/>
        <source>Arguments</source>
        <translation>パラメーター</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="187"/>
        <location filename="../../qml/console/RedisConsole.qml" line="198"/>
        <source>Description</source>
        <translation>説明</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="204"/>
        <source>Available since</source>
        <translation>利用可能</translation>
    </message>
    <message>
        <location filename="../../qml/console/RedisConsole.qml" line="255"/>
        <source>Close</source>
        <translation>閉じる</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="53"/>
        <source>Redis Version</source>
        <translation>Redisバージョン</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="68"/>
        <source>Used memory</source>
        <translation>消費メモリ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="83"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="391"/>
        <source>Clients</source>
        <translation>クライアント数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="98"/>
        <source>Commands Processed</source>
        <translation>実行コマンド数</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="115"/>
        <source>Uptime</source>
        <translation>稼働時間</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="135"/>
        <source> day(s)</source>
        <translation> 日</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="155"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="162"/>
        <source>Memory Usage</source>
        <translation>メモリ消費量</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="174"/>
        <source>Mb</source>
        <translation>MB</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="248"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="345"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="403"/>
        <source>Auto Refresh</source>
        <translation>自動リフレッシュ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="271"/>
        <source>Property</source>
        <translation>プロパティ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="277"/>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="17"/>
        <source>Value</source>
        <translation>値</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="333"/>
        <source>Slowlog</source>
        <translation>Slowlog</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="450"/>
        <source>Pub/Sub Channels</source>
        <translation>Pub/Subチャンネル</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="462"/>
        <source>Enable</source>
        <translation>有効</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="477"/>
        <source>Channel Name</source>
        <translation>チャンネル名</translation>
    </message>
    <message>
        <location filename="../../qml/GlobalSettings.qml" line="181"/>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="360"/>
        <source>Command</source>
        <translation>コマンド</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="377"/>
        <source>Processed at</source>
        <translation>処理</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="383"/>
        <source>Execution Time (μs)</source>
        <translation>実行時間(μs)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="418"/>
        <source>Client Address</source>
        <translation>クライアントのアドレス</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="424"/>
        <source>Age (sec)</source>
        <translation>経過時間(秒)</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="430"/>
        <source>Idle</source>
        <translation>アイドル</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="436"/>
        <source>Flags</source>
        <translation>フラグ</translation>
    </message>
    <message>
        <location filename="../../qml/server-info/ServerInfoTabs.qml" line="442"/>
        <source>Current Database</source>
        <translation>現在のデータベース</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="11"/>
        <source>Add New Key to </source>
        <translation>新しいキーを追加 </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="27"/>
        <location filename="../../qml/value-editor/editors/HashItemEditor.qml" line="17"/>
        <source>Key:</source>
        <translation>キー:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="37"/>
        <source>Type:</source>
        <translation>型:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="68"/>
        <location filename="../../qml/value-editor/ValueTabs.qml" line="708"/>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/AddKeyDialog.qml" line="114"/>
        <location filename="../../qml/value-editor/editors/formatters/formatters.js" line="75"/>
        <source>Error</source>
        <translation>エラー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/Pagination.qml" line="10"/>
        <source>Page</source>
        <translation>ページ</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="50"/>
        <source>Enter valid value</source>
        <translation>有効な値を入力</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="234"/>
        <source>Unknown formatter error (Empty response)</source>
        <translation>予期せぬフォーマッタのエラー　(空の応答)</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="262"/>
        <source>[Binary]</source>
        <translation>[バイナリ]</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="263"/>
        <source> [Compressed: </source>
        <translation> [圧縮: </translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="268"/>
        <source>Copy to Clipboard</source>
        <translation>クリップボードにコピー</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="277"/>
        <source>View as:</source>
        <translation>表示形式:</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/MultilineEditor.qml" line="296"/>
        <source>Large value (&gt;150kB). Formatters is not available.</source>
        <translation>値が大きすぎます(&gt;150kB)。フォーマッタは使用できません。</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="17"/>
        <location filename="../../qml/value-editor/editors/SortedSetItemEditor.qml" line="29"/>
        <source>Score</source>
        <translation>ソース</translation>
    </message>
    <message>
        <source>Invalid JSON: </source>
        <translation type="vanished">不正なJSON: </translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="180"/>
        <source>Path to dump.rdb file</source>
        <translation>dump.rdbファイルへのパス</translation>
    </message>
    <message>
        <location filename="../../qml/bulk-operations/BulkOperationsDialog.qml" line="182"/>
        <source>Select dump.rdb</source>
        <translation>dump.rdbを選択</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="17"/>
        <source>ID</source>
        <translation>ID</translation>
    </message>
    <message>
        <location filename="../../qml/value-editor/editors/StreamItemEditor.qml" line="58"/>
        <source>Value (represented as JSON object)</source>
        <translation>値(JSONオブジェクト形式)</translation>
    </message>
    <message>
        <location filename="../../app/models/key-models/listkey.cpp" line="83"/>
        <source>The row has been changed on server.Reload and try again.</source>
        <translation>サーバ上のROWが更新されました。リロードしてからやりなおしてください。</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/bulkoperationsmanager.cpp" line="122"/>
        <source>Failed to perform actions on %1 keys. </source>
        <translation>%1キーに対する処理が失敗しました</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="30"/>
        <source>Cannot copy key </source>
        <translation>キーをコピーできません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="84"/>
        <source>Cannot connect to source redis-server: %1</source>
        <translation>コピー元のredis-serverに接続できません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/copyoperation.cpp" line="98"/>
        <source>Cannot connect to target redis-server: %1</source>
        <translation>コピー先のredis-serverに接続できません: %1</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/deleteoperation.cpp" line="45"/>
        <source>Cannot remove key </source>
        <translation>キーを削除できません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/rdbimport.cpp" line="81"/>
        <source>Cannot execute command </source>
        <translation>コマンドを実行できません</translation>
    </message>
    <message>
        <location filename="../../modules/bulk-operations/operations/ttloperation.cpp" line="39"/>
        <source>Cannot set TTL for key </source>
        <translation>TTLをキーに設定できません</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/abstractnamespaceitem.cpp" line="165"/>
        <source>Your redis-server doesn&apos;t support &lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt; commands.</source>
        <translation>あなたのRedisサーバは&lt;a href=&apos;https://redis.io/commands/memory-usage&apos;&gt;&lt;b&gt;MEMORY&lt;/b&gt;&lt;/a&gt;コマンドをサポートしていません。</translation>
    </message>
    <message>
        <location filename="../../modules/connections-tree/items/namespaceitem.cpp" line="97"/>
        <source>Key was added. Do you want to reload keys in selected namespace?</source>
        <translation>キーを追加しました。選択されているネームスペースのキーをリロードしますか?</translation>
    </message>
    <message>
        <source>Cannot login - %1. &lt;br/&gt; Please try again or contact  &lt;a href=&apos;mailto:support@redisdesktop.com&apos;&gt;support@redisdesktop.com&lt;/a&gt;</source>
        <translation type="vanished">ログインできません - %1。&lt;br/&gt; もう一度実行するか、&lt;a href=&apos;mailto:support@redisdesktop.com&apos;&gt;support@redisdesktop.com&lt;/a&gt;までご連絡ください。</translation>
    </message>
    <message>
        <location filename="../../qml/common/FilePathInput.qml" line="25"/>
        <source>Select File</source>
        <translation>ファイルを選択</translation>
    </message>
</context>
</TS>
