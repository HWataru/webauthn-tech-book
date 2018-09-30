
= FIDO と パスワード認証の限界

== パスワードの限界

パスワードは容易に実装でき、機器に依存しない認証方法として、多くのサービスで使われている古の技術です。
しかしパスワードを利用した認証には多くの問題があります。
特に大きな問題となっているのが、フィッシングサイトとパスワードの使いまわしによるリスト型攻撃です。

パスワード認証では、入力したパスワードをサーバーに保存されているパスワードと照合することで認証します。
通常、パスワードは一方向ハッシュ関数でハッシュ化され保存されているため、仮に脆弱なサービスからパスワードのハッシュが流出しても、元のパスワードを求めることは不可能です。
しかし、すべてのサービスがパスワード認証をセキュア実装しているかというと、そうではなく Salt を利用していない、脆弱なハッシュ関数を利用している、といった
プログラム的な脆弱性や、パスワードリセット機能がブルートフォースアタック可能であるといった、機能的な脆弱性を突かれパスワードが流出するといった事例が後を絶ちません。

#@# パスワードの流出原因ではないか…
もうひとつパスワードの流出原因として挙げられるのがフィッシングサイトです。
巧妙に作られたフィッシングサイトは見分けることが難しく、単純なパスワード認証のみでは攻撃を防ぐことは困難です。
そのため最近は email, SMS などを利用した2段階認証の導入が進められています。
しかし、email などを利用した OTP ベースの2段階認証では、フィッシングに対するリスクの軽減にはなるものの、
攻撃者が実際のサービスに対し通信を行う、いわゆる中間者攻撃に対してはあまり効果がありません。

その結果、アカウントハックされ次のフィッシング攻撃の踏み台となってしまったり、
パスワードの流出が起き、関連するアカウントへのリスト型攻撃で情報漏洩や不正利用の被害が拡大します。

== FIDO Alliance

FIDO Alliance @<fn>{fidoalliance} は Fast Identity Online の略で、パスワードに依存しない新しい認証仕様の普及を目的とした団体です。

ボードメンバーとしては、グローバルでは Google, Microsoft, Amazon, Paypal といったリードカンパニーが参画しています。また今年（2018年3月）には Facebook がボードメンバー@<fn>{boardmember} として参画しています。
Alliance 内では仕様の策定や、FIDO Certificate の認定プログラム、仕様の普及のためのイベント等が、複数のワーキンググループに分かれて行われています。
日本国内では主に FIDO Japan WorkGroup というワーキンググループを中心に普及活動等をしています。


//footnote[fidoalliance][FIDO Alliance: @<href>{https://fidoalliance.org/}] 
//footnote[boardmember][2018/9現在 ボードレベル36,スポンサーレベル66,アソシエイトレベル151]

== FIDO の 3つの技術仕様

FIDO Alliance では U2F と UAF のふたつの仕様がすでに存在します。
FIDO UAF は Universal Authentication Framework の略で、主にスマートフォンでパスワードレス認証を行うための仕様です。
一方 U2F は、Universal 2nd Factor の略で、USBなどのいわゆる「ドングル」を利用して、 Web での2段階認証を行うための仕様です。

そして、本書で開設する Web Authentication API は、UAF/U2F を FIDO1.0 とするならば、FIDO2 プロジェクトの一部で、
U2F をベースとしつつも、Web でもスマートフォンでも利用可能なパスワードレス認証の仕様となっています。

== FIDO の基本コンセプト

FIDO の基本コンセプトは、シンプルな操作で、セキュアにログインができるエコシステムです。

FIDO の認証プロトコルでは、次の4つの要素でエコシステムの構築を試みました。

    1. 公開鍵暗号を用いたチャレンジレスポンス認証
    2. Authenticator を用いた秘密鍵の生成と保護
    3. origin に紐づく認証情報の管理
    4. ローカルでのユーザー認証

順番に説明していきましょう。

=== 公開鍵暗号を用いたチャレンジレスポンス認証

一般的なパスワード認証は、ユーザが入力したパスワードをサーバーに送信し、サーバーでは保存されたパスワード（一般的にはハッシュ化されている）とを比較して認証を行います。
しかし、前述したとおり、脆弱なサービスへの攻撃等でひとつのサービスからパスワードが漏洩してしまった場合、同じパスワードを使っている他のサービスまで影響を受けてしまいます。

一方、公開鍵を利用したチャレンジレスポンス認証では、サーバーから送られた challenge に、ユーザ－が所有する秘密鍵で署名し、その署名をサーバーに保存された公開鍵で検証します。

//image[challenge_response][チャレンジレスポンス認証]

ゆえにサーバー上には、ユーザーの公開鍵しか存在しないため、公開鍵が流出した場合においてもユーザーに成りすますことはできません。
FIDO で策定されているすべての仕様は、チャレンジレスポンス認証となっておりパスワード認証に比べセキュアに認証可能です。

=== Authenticator を用いた秘密鍵の生成と保護

FIDO の認証では、公開鍵暗号方式に必要なキーペアの管理を Authenticator が担います。Authenticator とは YubiKey といった外部デバイス、あるいは PC やスマートフォンといったデバイスです。

Authenticator とは、 YubiKey のような USB デバイスや、スマートフォンそのものであり、
ざっくりと U2F では YubiKey のような外部デバイスが Authenticator として動作し、
UAFではスマートフォンが、WebAuthn ではどちらも使えると捉えていただければよいかと思います。

FIDO 認証では Authenticator から秘密鍵が外部に漏れないようセキュアチップ上などに保存されます。

#@# 秘密鍵はセキュリティチップ上に保存するとは限らない？

=== origin に紐づく認証情報の管理

Authenticator は、各 origin ごとのキーペアの生成と Challenge への署名を行います。
origin ごとにキーペアを生成することで、中間者攻撃や、認証情報の串刺し検索でのプライバシー侵害を防ぐことができます。

たとえば、攻撃者がフィッシングサイトを作り、別の origin 用の認証リクエストをユーザーに送信したとしても、異なる origin に対して署名を送信することはありません。
あるいはユーザーが悪意ある origin 向けに作成したキーペア、あるいは署名では、正規の origin で認証ができません。
このように origin に紐づいた認証情報によって中間者攻撃をプロトコルレベルで防いでいます。

//image[mitm][origin bingind]

これは逆に、origin が異なるサービス間で認証情報の共有ができないことを意味します。
ゆえに現在異なる origin でサービスを提供している場合、Federation を利用した認証連携等を検討する必要があります。

そして Authenticator の役割として、もうひとつ、認証に利用するキーペアの生成、秘密鍵による署名を行う際のユーザー認証があります。

=== ローカルでのユーザー認証

Authenticator はキーペアの生成、秘密鍵での署名の際にユーザー認証を行います。
具体的には YubiKey にタッチする行為（User Presence）や、スマホの PIN や 指紋認証（User Verification）があります。

この認証の特徴的なのは、ユーザーと Authenticator との間で行われるローカルでの認証であることです。
よってユーザーは必ずデバイスを所持し明確な認証ジェスチャーを行わなければならず、遠隔からの操作で認証情報を盗まれるリスクが軽減されます。
また、デバイス上で生体認証でユーザーの本人確認をする際、その認証情報はデバイス内に保存され外部に漏れることはありません。

これによりサービス側は、ユーザーの生体情報などの機密情報を扱うことなく認証に利用することができます。
一方、ユーザー側は、自身の所有する Authenticator をタッチする、あるいは生体認証で認証する、といった簡単な操作で認証を完了できます。
裏側では、先ほど説明したとおりサービスごとのキーペア生成や、公開鍵暗号を用いたセキュアな認証が行われているのですが、ユーザーが意識する必要はありません。

//image[local_auth][ローカルでのユーザ認証]

== FIDO2

このような技術を利用して FIDO の基本コンセプトである シンプルな操作 と セキュアなログイン が実現されています。

この基本コンセプトをを理解したところで、本書のメインコンテンツである WebAuthn を含む FIDO2 プロジェクトについて解説したいと思います。

=== FIDO2 プロジェクトは

FIDO2 プロジェクトとは、本書で開設する W3Cの Web Authentication API（WebAuthn） と
Client と Authenticator との通信を規定した Client To Authenticator Protocol（CTAP）
で構成されているエコシステムの総称です。



=== 対応状況？

==== ブラウザ
==== プラットフォーム
==== Authenticator

=== まとめ