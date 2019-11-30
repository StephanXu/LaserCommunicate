# Laser Communicate Backend

If you need to need to run on `http://*:34500/`. Please run as Administrator to listen.

```cmd
netsh http add urlacl url=http://*:34500/ user=BUILTIN\Users listen=yes
```

And you also need to change the policy of Windows Firewall to enable inbound rule on the port you want to listen to and set its program to system.