const readline = require('readline');
const { Socket } = require('net');

const SOCKET_PATH = '../bin/endpoint';

let unix = new Socket({
   "allowHalfOpen": true
 });
let rl = readline.createInterface({
  "input": process.stdin,
  "output": process.stdout,
});

// For convinence
let ask = (query) => {
  return new Promise((resolve, reject) => {
    rl.question(query, (answer) => {
      resolve(answer);
    });
  });
};

unix.connect(SOCKET_PATH);

// Event handling
unix.on('connect', async () => {
  console.log('Connection established');
  unix.setEncoding('utf-8');
});
unix.on('data', async (data) => {
  console.log(data);
  let answer = await ask('Socket <<: ');
  unix.write(answer);
});
unix.on('error', (err) => {
  console.log(err);
});
// Bug: Close isn't detected
unix.on('close', () => {
  console.log('Done.');
  process.exit(0);
});
