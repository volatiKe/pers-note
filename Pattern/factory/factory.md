# 工厂模式

---

## 1. 工厂方法模式

```java
// 动物工厂
public abstract class AnimalFactory{

    // 获取动物
    public abstract Animal createAnimal();
}
```

```java
// 猫工厂
public class CatFactory extends AnimalFactory{

    @Overried
    // 获取猫
    public Animal createAnimal(){
        return new Cat();
    }
}
```

```java
// 狗工厂
public class DogFactory extends AnimalFactory{

    @Overried
    // 获取狗
    public Animal createAnimal(){
        return new Dog();
    }
}
```

```java
// 动物类
public abstract class Animal{

    public abstract void eat();
}
```

```java
// 更换动物种类时只需要更改工厂种类
AnimalFactory f = new xxxFactory();
Animal a = f.createAnimal();
a.eat();
```

* 可以看到在职责分配十分明确
* 增加新的对象时需要增加一个具体的类和对应的工厂

## 2. 简单工厂模式

```java
public class AnimalFactory {

    public static Animal createAnimal(String type) {
        if ("dog".equals(type)) {
            return new Dog();
        } else if ("cat".equals(type)) {
            return new Cat();
        }else{
            return null;
        }
    }
}
```

* 就是工厂方法模式的简化版
* 创建对象不暴露过程，可以通过工厂内部进行逻辑的封装：反射 / 动态代理
* 增加需要产出的对象时就需要修改工厂代码
