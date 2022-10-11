# http-server

简易HTTP服务器

## 编译

```bash
git clone https://github.com/moesnow/http-server.git
cd http-server
make
```

## 用法

```bash
./http-server <ip> <port>
```

## 示例

```bash
➜  ~ ./http-server 127.0.0.1 3000                               
```

```bash
➜  ~ curl http://127.0.0.1:3000                               
<html>
<head>
<title>title</title>
</head>
<body>Hello,World!</body>
</html>
```

