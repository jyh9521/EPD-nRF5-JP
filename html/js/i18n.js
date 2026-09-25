/* Web-controller language only. No BLE command or device setting is involved. */
const translations = {
  ja: {
    'EPD-nRF5-JP Web Controller': 'EPD-nRF5-JP ウェブコントローラー',
    '选择图片': '画像を選択',
    '语言': '言語', '蓝牙连接': 'Bluetooth 接続', '连接': '接続', '断开': '切断', '重连': '再接続',
    '清空日志': 'ログを消去', '驱动': 'ドライバー', '引脚': 'ピン', '确定': '確定',
    '设备控制': 'デバイス操作', '日历模式': 'カレンダーモード', '时钟模式': '時計モード',
    '清除屏幕': '画面を消去', '发送命令': 'コマンド送信', '蓝牙传图': 'Bluetooth 画像転送',
    '画布尺寸:': 'キャンバスサイズ:', '颜色模式:': 'カラーモード:',
    '双色(黑白)': '2色（白黒）', '三色(黑白红)': '3色（白黒赤）',
    '四色(黑白红黄)': '4色（白黒赤黄）', '六色(黑白红黄蓝绿)': '6色（白黒赤黄青緑）',
    '抖动算法:': 'ディザリング方式:', '无抖动': 'ディザリングなし',
    '抖动强度:': 'ディザリング強度:', '对比度:': 'コントラスト:',
    '确认间隔:': '確認間隔:', '状态：': '状態：', '旋转画布': 'キャンバスを回転',
    '清除画布': 'キャンバスを消去', '下载数组': '配列をダウンロード',
    '发送图片': '画像を送信', '颜色:': '色:', '黑色': '黒', '红色': '赤',
    '黄色': '黄', '绿色': '緑', '蓝色': '青', '白色': '白', '粗细:': '太さ:',
    '字体:': 'フォント:', '大小:': 'サイズ:', '添加文字': '文字を追加',
    '完成': '完了', '开发模式': '開発モード', '正常模式': '通常モード',
    '交流群': '交流グループ', '画笔': 'ブラシ', '橡皮擦': '消しゴム',
    '撤销 (Ctrl+Z)': '元に戻す (Ctrl+Z)', '重做 (Ctrl+Y)': 'やり直す (Ctrl+Y)',
    '输入文字': '文字を入力', '粗体': '太字', '斜体': '斜体',
    '放大': '拡大', '缩小': '縮小', '左移': '左へ', '上移': '上へ',
    '下移': '下へ', '右移': '右へ',
    '4.2寸': '4.2インチ', '5.83寸低分': '5.83インチ低解像度',
    '5.83寸': '5.83インチ', '7.5寸低分': '7.5インチ低解像度',
    '7.5寸': '7.5インチ', '黑白': '白黒', '三色': '3色', '四色': '4色',
    '服务不可用，请检查蓝牙连接': 'サービスを利用できません。Bluetooth 接続を確認してください。',
    '数据块': 'データブロック', '红色块': '赤色ブロック',
    '总用时:': '経過時間:',
    '提醒：时钟模式目前使用全刷实现，此功能目前多用于修复老化屏残影问题，不建议长期开启，是否继续？':
      '注意：時計モードは全面更新を使用します。残像の改善向けで、長時間の使用は推奨しません。続行しますか？',
    '时间已同步！': '時刻を同期しました。', '屏幕刷新完成前请不要操作。': '画面の更新が終わるまで操作しないでください。',
    '确认清除屏幕内容?': '画面の内容を消去しますか？', '清屏指令已发送！': '画面消去コマンドを送信しました。',
    '请先完成图片裁剪！发送已取消。': '先に画像のトリミングを完了してください。送信を中止しました。',
    '警告：画布尺寸和驱动不匹配，是否继续？': '警告：キャンバスサイズとドライバーが一致しません。続行しますか？',
    '警告：颜色模式和驱动不匹配，是否继续？': '警告：カラーモードとドライバーが一致しません。続行しますか？',
    '当前固件不支持此颜色模式。': '現在のファームウェアはこのカラーモードに対応していません。',
    '发送完成！耗时:': '送信完了！所要時間:',
    '请先完成图片裁剪！下载已取消。': '先に画像のトリミングを完了してください。ダウンロードを中止しました。',
    '数组大小不匹配。请检查图像尺寸和模式。': '配列サイズが一致しません。画像サイズとモードを確認してください。',
    '已断开连接.': '接続を切断しました。', '正在重连': '再接続しています',
    '请检查蓝牙是否已开启，且使用的浏览器支持蓝牙！建议使用以下浏览器：':
      'Bluetooth が有効で、ブラウザーが Web Bluetooth に対応しているか確認してください。推奨ブラウザー：',
    '• 电脑: Chrome/Edge': '• PC: Chrome/Edge', '• Android: Chrome/Edge': '• Android: Chrome/Edge',
    '• iOS: Bluefy 浏览器': '• iOS: Bluefy ブラウザー',
    '收到配置：': '設定を受信：', 'MTU 已更新为:': 'MTU を更新:',
    '已开启 RLE 压缩传输支持': 'RLE 圧縮転送に対応しています',
    '远端时间:': 'デバイス時刻:', '本地时间:': 'ローカル時刻:',
    '正在连接:': '接続中:', '  找到 GATT Server': '  GATT サーバーを検出',
    '  找到 EPD Service': '  EPD サービスを検出', '  找到 Characteristic': '  Characteristic を検出',
    '固件版本:': 'ファームウェア版:', '旧版固件可能不支持部分功能。': '古いファームウェアでは一部の機能が使えない場合があります。',
    '图片宽高比例与画布不匹配，将进入裁剪模式。\n请放大图片后移动图片使其充满画布, 再点击"完成"按钮。':
      '画像の縦横比がキャンバスと異なるため、トリミングモードに入ります。\n画像を拡大・移動してキャンバスを満たし、「完了」を押してください。',
    '清除画布内容?': 'キャンバスの内容を消去しますか？',
    '注意：开发模式功能已开启！不懂请不要随意修改，否则后果自负！': '注意：開発モードが有効です。設定の変更には十分ご注意ください。',
    '画笔模式': 'ブラシモード', '橡皮擦': '消しゴム', '插入文字': '文字を挿入',
    '请输入文字内容': '文字を入力してください', '点击画布放置文字': 'キャンバスをクリックして文字を配置',
    '拖动新添加文字可调整位置': '追加した文字をドラッグして位置を調整',
    '裁剪模式: 可用鼠标滚轮或双指触摸缩放图片': 'トリミングモード：ホイールまたはピンチ操作で拡大・縮小',
    '本群是此开源固件作者的技术交流群\n如果你购买了成品，请找卖家提供售后！':
      'このグループはファームウェア作者の技術交流用です。\n完成品のサポートは販売元にお問い合わせください。',
    'device_setting': 'デバイスの設定: {day}', 'saving_device': 'デバイスに保存しています…',
    'save_failed': '保存に失敗しました。', 'week_initial': 'デバイスに接続すると現在の設定を読み込みます。',
    'request_device_failed': 'デバイスの選択に失敗しました。',
    'connect_failed': 'Bluetooth 接続に失敗しました。',
    'notification_failed': '通知の開始に失敗しました。',
    'write_failed': 'デバイスへの書き込みに失敗しました。',
  },
  'zh-CN': {
    'EPD-nRF5-JP Web Controller': 'EPD-nRF5-JP 网页控制器',
    'カレンダー設定': '日历设置', '週の開始曜日': '每周起始日',
    '日曜日': '星期日', '月曜日': '星期一', '設定を保存': '保存设置',
    'デバイスに接続すると現在の設定を読み込みます。': '连接设备后读取当前设置。',
    'device_setting': '设备设置：{day}', 'saving_device': '正在保存到设备…',
    'save_failed': '保存失败。', 'week_initial': '连接设备后读取当前设置。',
    'request_device_failed': '选择设备失败。', 'connect_failed': '蓝牙连接失败。',
    'notification_failed': '启动通知失败。',
    'write_failed': '写入设备失败。',
  }
};

/* Product/font names are identifiers; translate the visible descriptors only. */
const fontDisplayNames = {
  '宋体': 'SimSun', '黑体': 'SimHei', '微软雅黑': 'Microsoft YaHei',
  '微软正黑体': 'Microsoft JhengHei', '楷体': 'KaiTi', '新宋体': 'NSimSun',
  '仿宋': 'FangSong', '幼圆': 'YouYuan', '隶书': 'LiSu',
  '华文黑体': 'STHeiti', '华文细黑': 'STXihei', '华文楷体': 'STKaiti',
  '华文宋体': 'STSong', '华文仿宋': 'STFangsong', '华文中宋': 'STZhongsong',
  '华文琥珀': 'STHupo', '华文新魏': 'STXinwei', '华文隶书': 'STLiti',
  '华文行楷': 'STXingkai', '方正舒体': 'FZShuTi', '方正姚体': 'FZYaoti',
  '苹方': 'PingFang SC', '思源黑体': 'Source Han Sans CN',
  '思源宋体': 'Source Han Serif SC', '文泉驿微米黑': 'WenQuanYi Micro Hei',
};

const localeStorageKey = 'epd-jp-web-locale';
function browserLocale(languages = navigator.languages || [navigator.language]) {
  for (const language of languages.length ? languages : [navigator.language]) {
    if (/^ja(?:-|$)/i.test(language)) return 'ja';
    if (/^zh(?:-|$)/i.test(language)) return 'zh-CN';
  }
  return 'ja';
}
let webLocale = (() => {
  try {
    const saved = localStorage.getItem(localeStorageKey);
    if (saved === 'ja' || saved === 'zh-CN') return saved;
  } catch (_) { /* localStorage can be disabled; use browser preference. */ }
  return browserLocale();
})();

function t(key, params = {}) {
  let value = translations[webLocale][key] || (webLocale === 'ja' && fontDisplayNames[key]) || key;
  if (webLocale === 'ja') {
    value = value.replace(/(\d+(?:\.\d+)?)寸/g, '$1インチ')
      .replace(/低分/g, '低解像度').replace(/黑白/g, '白黒')
      .replace(/三色/g, '3色').replace(/四色/g, '4色');
  }
  return value.replace(/\{(\w+)\}/g, (_, name) => params[name] ?? '');
}

const staticTranslations = [];
function captureStaticTranslations() {
  const walker = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT);
  while (walker.nextNode()) {
    const node = walker.currentNode;
    if (['SCRIPT', 'STYLE'].includes(node.parentElement?.tagName)) continue;
    const match = node.nodeValue.match(/^(\s*)(\S[\s\S]*?\S|\S)(\s*)$/);
    if (match) staticTranslations.push({node, key: match[2], prefix: match[1], suffix: match[3]});
  }
  for (const element of document.querySelectorAll('[title], [placeholder], [aria-label]')) {
    for (const attribute of ['title', 'placeholder', 'aria-label']) {
      if (element.hasAttribute(attribute)) staticTranslations.push({element, attribute, key: element.getAttribute(attribute)});
    }
  }
}

function applyLocale(locale) {
  webLocale = locale === 'zh-CN' ? 'zh-CN' : 'ja';
  document.documentElement.lang = webLocale;
  document.title = t('EPD-nRF5-JP Web Controller');
  for (const item of staticTranslations) {
    if (item.node) item.node.nodeValue = item.prefix + t(item.key) + item.suffix;
    else item.element.setAttribute(item.attribute, t(item.key));
  }
  const selector = document.getElementById('languageSelect');
  if (selector) selector.value = webLocale;
  if (typeof refreshLocalizedState === 'function') refreshLocalizedState();
}

function setWebLocale(locale) {
  try { localStorage.setItem(localeStorageKey, locale); } catch (_) { /* Keep current session. */ }
  applyLocale(locale);
}

captureStaticTranslations();
applyLocale(webLocale);
