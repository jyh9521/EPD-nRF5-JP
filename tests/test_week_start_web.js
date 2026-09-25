const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const path = require('node:path');

const elements = new Map();
const document = {
  body: {},
  getElementById(id) {
    if (!elements.has(id)) elements.set(id, { value: '', disabled: false, textContent: '', innerHTML: '' });
    return elements.get(id);
  },
};

const context = vm.createContext({ document, console, Uint8Array, TextDecoder });
const source = fs.readFileSync(path.join(__dirname, '..', 'html', 'js', 'main.js'), 'utf8');
vm.runInContext(source, context, { filename: 'main.js' });
vm.runInContext('addLog = () => {}; updateDitcherOptions = () => {}; gattServer = { connected: true };', context);

const writes = [];
context.mockWrites = writes;

function notifyWeekStart(day, index) {
  const config = new Uint8Array([0x14, 0x13, 0x06, 0x05, 0x04, 0x03, 0x02, 0x03,
    0xff, 0x12, 0x07, 0x01, day]);
  context.payload = new DataView(config.buffer);
  vm.runInContext(`handleNotify(payload, ${index});`, context);
}

(async () => {
  vm.runInContext('resetVariables();', context);
  vm.runInContext(`
    const testCharacteristic = {
      async startNotifications() {},
      addEventListener() {},
      async writeValueWithResponse(payload) { mockWrites.push(Array.from(payload)); },
      async writeValueWithoutResponse(payload) { mockWrites.push(Array.from(payload)); }
    };
    bleDevice = { name: 'NRF_EPD_TEST', gatt: {
      connected: true,
      async connect() { return {
        connected: true,
        async getPrimaryService() { return {
          async getCharacteristic(uuid) {
            if (uuid.startsWith('62750003')) return { async readValue() { return new DataView(new Uint8Array([0x1a]).buffer); } };
            return testCharacteristic;
          }
        }; }
      }; }
    } };
  `, context);
  await vm.runInContext('connect()', context);
  assert.deepEqual(Array.from(writes.at(-1)), [0x22]); // missed initial push is recovered explicitly
  notifyWeekStart(0, 0);
  assert.equal(document.getElementById('weekstart').value, '0');
  assert.equal(document.getElementById('weekstartstatus').textContent, 'デバイスの設定: 日曜日');

  document.getElementById('weekstart').value = '1';
  await vm.runInContext('setWeekStart()', context);
  assert.deepEqual(Array.from(writes.at(-2)), [0x21, 0x01]);
  assert.deepEqual(Array.from(writes.at(-1)), [0x22]);
  notifyWeekStart(1, 2);
  assert.equal(document.getElementById('weekstart').value, '1');
  assert.equal(document.getElementById('weekstartstatus').textContent, 'デバイスの設定: 月曜日');

  vm.runInContext('resetVariables();', context);
  assert.equal(document.getElementById('weekstart').value, '0');
  await vm.runInContext('connect()', context);
  assert.deepEqual(Array.from(writes.at(-1)), [0x22]);
  notifyWeekStart(1, 0); // a fresh browser reads the device, not localStorage
  assert.equal(document.getElementById('weekstart').value, '1');
  assert.equal(document.getElementById('weekstart').disabled, false);
  console.log('Web week-start BLE/UI test: PASS');
})().catch((error) => { console.error(error); process.exitCode = 1; });
