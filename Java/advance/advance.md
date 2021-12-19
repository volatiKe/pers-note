# 高级特性

---

## 1. RTTI 与反射

RTTI 就是 Runtime Type Information，也就是说 Java 能够在运行时判断或获取某个对象的类型信息，这个过程需要借助 Class 对象，因为对象的类型信息存放在其所属类的 Class 对象中。

RTTI 有两种形式：

* 传统的 RTTI，如多态：无论对引用进行怎样的类型转换，对象对应的 Class 对象只有一个，那么在运行时就可以通过这个 Class 对象，找到方法表，查看子类重写父类方法的情况，从而保证方法调用的正确性
* 反射

获取 Class 对象的方式：

* 对象。getClass()
* 类。class
* Class.forName("类名")

以上三种方式获取到的 Class 对象是相同的，即通过「==」比较结果为 true。

需要注意的是，反射和传统 RTTI 的区别在于如何去操作 Class 对象：

* 前两种方法必须保证编译通过并且类已经加载，属于传统的 RTTI
* 第三种方式就是通常意义上的反射，直接通过类名在方法区寻找对 Class 对象的引用，注意这种方式下类可以不事先加载，因为这种方式会触发类加载

## 2. 泛型

Java 的泛型属于一种语法糖，目的是在编译期间进行类型检查，避免类型转换错误。

### 2.1 泛型擦除

编译后的代码会被擦除类型信息。由于泛型是 JDK5 之后引入的，为了兼容旧代码，所以使用了泛型擦除机制。

限制：

* Java 不允许创建泛型数组：如支持泛型的容器无法直接创建数组。因为数组根据元素类型申请同样大小的连续地址空间，为了避免放入其他类型元素导致空间不够，所以数组会在运行期进行类型检查，而泛型的类型信息在运行期被擦除，与数组的检查机制冲突。可以借助强转的方式绕过这种限制：

  ```java
  List<Integer>[] genericArray = (List<Integer>[])new ArrayList[10];
  ```

* 无法创建泛型实例：可以借助反射创建泛型实例

  ```java
  public static <E> void append(List<E> list) {
    E elem = new E();  // 编译失败
    list.add(elem);
  }

  public static <E> void append(List<E> list, Class<E> cls) throws Exception {
      E elem = cls.newInstance();
      list.add(elem);
  }
  List<String> ls = new ArrayList<>();
  append(ls, String.class);
  ```

### 2.2 PECS 原则

首先需要明确的是：正因为泛型信息会在编译后擦除，所以即使 Apple extends Fruit，List\<Fruit> 和 List\<Apple> 之间也不存在任何继承关系，这就需要 PECS 原则提升 API 的灵活性。

#### 2.2.1 PESC 下容器的声明

假设 Child extends T extends Parent：

```java
List<? extends Parent> testPT1 = new ArrayList<T>();
List<? extends Parent> testPC1 = new ArrayList<Child>();
List<? extends T> testTP1 = new ArrayList<Parent>();//报错
List<? extends T> testTC1 = new ArrayList<Child>();
List<? extends Child> testCP1 = new ArrayList<Parent>();//报错
List<? extends Child> testCT1 = new ArrayList<T>();//报错

List<? super Parent> testPT2 = new ArrayList<T>();//报错
List<? super Parent> testPC2 = new ArrayList<Child>();//报错
List<? super T> testTP2 = new ArrayList<Parent>();
List<? super T> testTC2 = new ArrayList<Child>();//报错
List<? super Child> testCP2 = new ArrayList<Parent>();
List<? super Child> testCT2 = new ArrayList<T>();
```

**擦除的边界**：编译器虽然会擦除类型信息，但会保证类型一致性，所以类型参数会被擦除到一个边界

* `<? extends T>` 界定了最大父类是 T，即实际指向必须是其或子类
* `<? super T>` 界定了最小子类是 T，即实际指向必须是其或父类

#### 2.2.2 PESC 下容器的读写

假设 Child extends T extends Parent：

```java
List<? extends T> extendsT = new ArrayList<>();
extendsT.add(new Parent());//报错
extendsT.add(new T());//报错
extendsT.add(new Child());//报错
Parent p1 = extendsT.get(0);
T t1 = extendsT.get(0);
Child c1 = extendsT.get(0);//报错

List<? super T> superT = new ArrayList<>();
superT.add(new Parent());//报错
superT.add(new T());
superT.add(new Child());
Parent p2 = superT.get(0);//报错
T t2 = superT.get(0);//报错
Child c2 = superT.get(0);//报错
```

* `<? extends T>`
  * 只可按照 T 或 T 的父类类型读取：因为存入的均为 T 及 T 的子类，按父类读取不会有问题
  * 不可写入：容器存放的是T及其子类，泛型擦除后编译器
    ```

* `<? super Apple>`
  * 会将容器声明为只写。因为编译器知道存放的元素是 Apple 或其父类，但不知道具体类型，所以读取时按照 Object 返回一定不会出错，而写入时则不允许加入任何父类，但可以写入 T 或其子类，因为 T 的子类一定可以向上转型为 T
