# 代理模式

---

## 1. 静态代理

```java
public interface User{

    void say();
}
```

```java
public class ActualUser implements User{

    @Override
    public void say(){
         System.out.println("我是个杠精，你来打我啊");
    }
}
```

```java
// 真实用户喜欢抬杠，现实生活中抬杠容易被打，在网上不会
public class VirtualUser implements User{

    private User user;

    public VirtualUser(User user){
        this.user = user;
    }

    private void login(){
        System.out.println("登录帐号");
    }

    @Override
    public void say(){
        login();
        user.say();
    }
}
```

运行：

```java
// 想要抬杠的真实用户
User actualUser = new ActualUser();
// 网上的虚拟用户
User virtualUser = new VirtualUse(actualUser);
// 在网上抬杠
virtualUser.say();
```

静态代理需要代理类和代理类实现同一个接口，并且代理类持有被代理类的实例。

## 2. 动态代理

### 2.1 JDK 中的动态代理

```java
public class DynamicUser implements InvocationHandler {

    private Object object;

    public Object getProxyObject(Object object) {
        this.object = object;
        // Proxy.newProxyInstance(接口类.class.getClassLoader(), new Class[]{接口类.class}, h)
        // loader：被代理类的 ClassLoder
        // interfaces：被代理类实现的接口
        // h：InvocationHandler 对象，表示当这个动态代理对象在被调用方法的时候，会转发到哪一个InvocationHandler 对象上，间接通过 invoke() 来执行
        return Proxy.newProxyInstance(object.getClass().getClassLoader(), object.getClass().getInterfaces(), this);
    }

    private void login() {

        System.out.println("login");
    }

    @Override
    // proxy：代理类的对象
    // method：将要执行的方法
    // args：方法的参数
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        login();
        // object：被代理类的对象
        return method.invoke(object, args);
    }
}
```

运行：

```java
User actualUser = new ActualUser();
// 获得动态代理类
User dynamicUser = (User) new DynamicUser().getProxyObject(actualUser);
dynamicUser.say();
```

如何理解「动态」？

JDK 中实现的动态代理相比于静态代理，优势是在于在被代理类和接口还未知的时候就能确定对方法需要增加的行为，就像上边给出的示例，单看 InvocationHandler 的实现，即使脱离 User 接口和 ActualUser 这个被代理类，仍然不会存在任何问题，而具体代理哪个类是在获取代理类对象时才决定的：

* 传入不同的被代理类对象
* 対获得的代理对象进行强转

相比之下静态代理的代理类实现是和被代理类绑定的，被实现的接口限制了，当需要给多个被代理类添加相同功能时需要修改每个被代理类，而动态代理只需要修改 InvocationHandler 的实现即可。

### 2.2 JDK 动态代理的原理

动态代理的核心显然是 Proxy.newProxyInstance()，内部的调用链不是关注的重点，重点是最后生成了代理类的字节码，通过配置 JVM 参数：

```shell
-Dsun.misc.ProxyGenerator.saveGeneratedFile=true
```

能够保存动态代理类的 class 文件到磁盘，下边是上一节中示例代码的动态代理类：

```java
//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.sun.proxy;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.lang.reflect.UndeclaredThrowableException;
import proxy.User;

public final class $Proxy0 extends Proxy implements User {
    private static Method m1;
    private static Method m2;
    private static Method m3;
    private static Method m0;

    // 这里调用了父类的构造，拿到了我们实现的 InvocationHandler 的实例
    public $Proxy0(InvocationHandler var1) throws  {
        super(var1);
    }

    public final boolean equals(Object var1) throws  {
        try {
            return (Boolean)super.h.invoke(this, m1, new Object[]{var1});
        } catch (RuntimeException | Error var3) {
            throw var3;
        } catch (Throwable var4) {
            throw new UndeclaredThrowableException(var4);
        }
    }

    public final String toString() throws  {
        try {
            return (String)super.h.invoke(this, m2, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    public final void say() throws  {
        try {
            super.h.invoke(this, m3, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    public final int hashCode() throws  {
        try {
            return (Integer)super.h.invoke(this, m0, (Object[])null);
        } catch (RuntimeException | Error var2) {
            throw var2;
        } catch (Throwable var3) {
            throw new UndeclaredThrowableException(var3);
        }
    }

    // 类初始化时会执行这个代码块中的内容，可以看到通过反射拿到了所有被代理类实现的接口的方法
    static {
        try {
            m1 = Class.forName("java.lang.Object").getMethod("equals", Class.forName("java.lang.Object"));
            m2 = Class.forName("java.lang.Object").getMethod("toString");
            m3 = Class.forName("proxy.User").getMethod("say");
            m0 = Class.forName("java.lang.Object").getMethod("hashCode");
        } catch (NoSuchMethodException var2) {
            throw new NoSuchMethodError(var2.getMessage());
        } catch (ClassNotFoundException var3) {
            throw new NoClassDefFoundError(var3.getMessage());
        }
    }
}
```

* 这个代理类继承了 Proxy，Proxy 持有 InvocationHandler，当我们传入它的实例时 (从 newProxyInstance() 传入)，Proxy 就持有了我们自己实现的 InvocationHandler 的实例，在这个动态代理类中就能够访问
* 通过反射获得到了所有的被代理类实现的接口的方法，如 m3 对应了 say()
* 代理类会实现被代理类的接口，会重写所有接口方法，而重写的逻辑就是通过 `h.invoke()` 来将调用转发到我们自行实现的 invoke() 上

## 3. 其他代理

CGLib：相比与 JDK 动态代理的被代理类必须实现接口，CGLib没有这个限制，通过继承重写方法，动态生成字节码实现代理，但是由于使用继承的方式，会受到 static、final、private 的影响。

AspectJ：在编译字节码的时候就在方法周围加上业务逻辑，但是需要特定的编译器的支持。

Spring 中的代理：Spring 底层使用的仍然是 JDK 和 CGLib 的代理，只是引入了 AOP 的概念（aspect、advice、point等）和 AspectJ 的一些注解（@PointCut、@After、@Before等），通过这些対代理进行了封装。
