const electron = require('electron')
const {
    app,
    BrowserWindow,
    ipcMain
} = require('electron')
const fs = require('fs');
const configPath = app.getPath('userData');
let mainWindow;

const initConfig =
    '{ \
    "rules": []\
}';

function createWindow() {
    mainWindow = new BrowserWindow({
        width: 960,
        height: 720,
        frame: false,
        webPreferences: {
            nodeIntegration: true
        },
        backgroundColor: '#ffffff'
    })

    // 加载index.html文件
    mainWindow.loadURL('http://localhost:8080')
    // mainWindow.loadFile('index.html')

    // 监听浏览器窗口对象是否关闭，关闭之后直接将mainWindow指向空引用，也就是回收对象内存空间
    mainWindow.on("closed", function () {
        mainWindow = null;
    })

    mainWindow.openDevTools({
        mode: 'bottom'
    })
}

app.on('ready', createWindow)

ipcMain.on('test_msg', (event, arg) => {
    console.log('rec msg')
})

ipcMain.on('controlBtn', (event, arg) => {
    if (arg === 'shutdown') {
        mainWindow.close();
    } else if (arg === 'mini') {
        mainWindow.minimize();
    } else if (arg === 'max') {
        if (mainWindow.isMaximized()) {
            mainWindow.restore();
        } else {
            mainWindow.maximize();
        }
    }
})

ipcMain.on('acquireData', (event, arg) => {

})

ipcMain.on('saveConfigs', (event, arg) => {
    fs.writeFile(configPath + '/userConfigs.json', arg, function (err) {
        if (err) {
            event.returnValue = 'fail'
            throw err;
        }
        event.returnValue = 'suc'
    });
});

ipcMain.on('loadConfigs', (event, arg) => {
    let data = '';
    try {
        data = fs.readFileSync(configPath + '/userConfigs.json', 'utf-8');
    } catch (err) {
        console.log(err);
        let initJsonContent = initConfig;
        fs.writeFileSync(configPath + '/userConfigs.json', initJsonContent);
        data = initJsonContent;
    }
    event.returnValue = data;
});