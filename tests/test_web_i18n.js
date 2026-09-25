const assert = require('assert');
const fs = require('fs');
const vm = require('vm');

const code = fs.readFileSync('html/js/i18n.js', 'utf8') +
  '\nglobalThis.i18nTest = {browserLocale, setWebLocale, t, translations, fontDisplayNames};';

function boot(languages, saved) {
  const storage = new Map(saved ? [['epd-jp-web-locale', saved]] : []);
  const nodes = ['蓝牙连接', 'カレンダー設定', '连接', 'デバイスに接続すると現在の設定を読み込みます。']
    .map(value => ({nodeValue: value, parentElement: {tagName: 'DIV'}}));
  const elements = {languageSelect: {value: ''}};
  const document = {
    body: {}, documentElement: {lang: ''}, title: '',
    createTreeWalker() {
      let index = -1;
      return {get currentNode() {return nodes[index];}, nextNode() {return ++index < nodes.length;}};
    },
    querySelectorAll() {return [];},
    getElementById(id) {return elements[id] || null;},
  };
  let bleWrites = 0;
  const context = {
    navigator: {languages, bluetooth: {requestDevice() {bleWrites++;}}},
    localStorage: {getItem: key => storage.get(key) || null, setItem: (key, value) => storage.set(key, value)},
    document, NodeFilter: {SHOW_TEXT: 4},
  };
  vm.runInNewContext(code, context);
  return {api: context.i18nTest, document, nodes, elements, storage, get bleWrites() {return bleWrites;}};
}

for (const language of ['ja', 'ja-JP', 'en-US']) {
  const page = boot([language]);
  assert.strictEqual(page.document.documentElement.lang, 'ja');
  assert.strictEqual(page.nodes[0].nodeValue, 'Bluetooth 接続');
  assert.strictEqual(page.nodes[2].nodeValue, '接続');
}
for (const language of ['zh', 'zh-CN', 'zh-TW', 'zh-HK']) {
  assert.strictEqual(boot([language]).document.documentElement.lang, 'zh-CN');
}
const page = boot(['en-US'], 'zh-CN');
assert.strictEqual(page.document.documentElement.lang, 'zh-CN');
assert.strictEqual(page.nodes[1].nodeValue, '日历设置');
page.api.setWebLocale('ja');
assert.strictEqual(page.document.documentElement.lang, 'ja');
assert.strictEqual(page.elements.languageSelect.value, 'ja');
assert.strictEqual(page.storage.get('epd-jp-web-locale'), 'ja');
assert.strictEqual(page.nodes[1].nodeValue, 'カレンダー設定');
assert.strictEqual(page.api.t('device_setting', {day: '日曜日'}), 'デバイスの設定: 日曜日');
assert.strictEqual(page.api.t('时间已同步！'), '時刻を同期しました。');
page.api.setWebLocale('zh-CN');
assert.strictEqual(page.api.t('device_setting', {day: '星期日'}), '设备设置：星期日');
assert.strictEqual(page.bleWrites, 0);

let dynamicCount = 0;
for (const file of ['html/js/main.js', 'html/js/paint.js', 'html/js/crop.js']) {
  const source = fs.readFileSync(file, 'utf8');
  for (const [, , key] of source.matchAll(/\bt\((['"])(.*?)\1/g)) {
    dynamicCount++;
    const decoded = key.replace(/\\n/g, '\n');
    assert(page.api.translations.ja[decoded] || page.api.translations['zh-CN'][decoded], `Untranslated dynamic key ${file}: ${key}`);
  }
}
assert(dynamicCount >= 50);
const html = fs.readFileSync('html/index.html', 'utf8').replace(/<script\b[^>]*>[\s\S]*?<\/script>/g, '');
const staticKeys = [...html.matchAll(/>([^<>]+)</g)].map(([, value]) => value.trim())
  .concat([...html.matchAll(/(?:title|placeholder|aria-label)="([^"]+)"/g)].map(([, value]) => value))
  .filter(value => /[\u3040-\u9fff]/.test(value));
const uniqueStatic = new Set(staticKeys);
for (const key of uniqueStatic) {
  if (key === '日本語' || key === '简体中文') continue; // Language autonyms.
  assert(page.api.translations.ja[key] || page.api.translations['zh-CN'][key] ||
    page.api.fontDisplayNames[key] || /(寸|黑白|三色|四色)/.test(key),
  `Untranslated static text: ${key}`);
}
assert(uniqueStatic.size >= 100);
console.log(`Web i18n tests: PASS (locale detection, immediate switch, persistence, HTML lang, ${uniqueStatic.size} static labels, ${dynamicCount} dynamic call sites, no BLE write)`);
