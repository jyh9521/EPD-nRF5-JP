const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const source = fs.readFileSync(path.join(__dirname, '..', 'html', 'js', 'main.js'), 'utf8');
const page = fs.readFileSync(path.join(__dirname, '..', 'html', 'index.html'), 'utf8');
const service = fs.readFileSync(path.join(__dirname, '..', 'EPD', 'EPD_service.c'), 'utf8');
const header = fs.readFileSync(path.join(__dirname, '..', 'EPD', 'EPD_service.h'), 'utf8');
assert.match(service, /#define EPD_CFG_52811 \{[^}]*0x02, 0x03, 0xFF/);
assert.match(header, /#define APP_VERSION 0x1d\b/);
assert.match(page, /js\/main\.js\?v=1d/);
if (process.argv[2]) {
  const binary = fs.readFileSync(process.argv[2]);
  assert.notEqual(binary.indexOf(Buffer.from('1413060504030203ff1207', 'hex')), -1);
}
const context = vm.createContext({ document: { body: {} }, console, calls: [] });
vm.runInContext(source, context);

const check = expression => vm.runInContext(expression, context);
assert.equal(check('shouldAutoSyncClock(0x1d, 1, 1000, 1100, 0)'), true);
assert.equal(check('shouldAutoSyncClock(0x1d, 2, 1000, 1030, 0)'), false);
assert.equal(check('shouldAutoSyncClock(0x1d, 0, 1000, 1100, 0)'), false);
assert.equal(check('shouldAutoSyncClock(0x1c, 1, 1000, 1100, 0)'), false);
assert.equal(check('shouldAutoSyncClock(0x1d, 1, 3700, 100, -60)'), false);

check(`
  write = async (cmd, data) => { globalThis.calls.push([cmd, Array.from(data)]); return true; };
  addLog = () => {};
  t = key => key;
  gattServer = { connected: true };
  connectHandshakeComplete = true;
  autoSyncDone = false;
  appVersion = 0x1d;
  deviceDisplayMode = 1;
  remoteClockSeconds = 1735689694;
`);

(async () => {
  await check('maybeSyncConnectedTime()');
  assert.equal(context.calls.length, 1);
  assert.equal(context.calls[0][0], 0x20);
  assert.equal(context.calls[0][1].at(-1), 1);
  await check('maybeSyncConnectedTime()');
  assert.equal(context.calls.length, 1, 'one connection must not trigger duplicate refreshes');
  console.log('Startup tests: PASS (firmware model 03, version 0x1d, drift, preserved mode, single SET_TIME)');
})().catch(error => { console.error(error); process.exitCode = 1; });
