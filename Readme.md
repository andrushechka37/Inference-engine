# Машина вывода
Машина вывода — программа, которая выполняет логический вывод из предварительно построенной базы фактов и правил.

# Моя реализация машины вывода
## Как использовать?
```
git clone https://github.com/andrushechka37/Inference-engine.git
```
Переходим в директорию проекта.
```
mkdir obj
```

Записываем правила и факты в файл input.txt.

Пример:
```
a = 1;
b = 0;
c = 0;
-
(a & !b) ? c = 1, b = 1;
(c & !b) ? a = 0;
(!a & !c) ? b = 1;
```
Сначала идет присвоение переменных, в именах переменных могут использоваться только английские буквы. В конце каждого присвоения должна стоять точка с запятой.

После присвоения начальных значений должна стоять отделяющая черта `-`.

Правила строятся по следующим правилам:

`(`какое-то логическое выражение`)` `?` набор присвоений вида (a `=` 0), в случае нескольких переменных они перечисляются через запятую. В конце должна стоять точка с запятой.

После заполнения input.txt поочередно выполняем команды 
```
make rules
```
```
make engine
``` 
Отчет о работе машины находится в файле report.txt:
```
the rule № 1 worked
a: 1 -> 1
b: 0 -> 1
c: 0 -> 1

------------------

a: 1 -> 1
b: 1 -> 1
c: 1 -> 1

------------------
```

## Нюансы реализации
Для игнорирования пробельных символов была применена токенизация ввода. 

Написанные правила транслируются в C, чтобы их include`ить при выполнении программы. Возможно это не самая очевидная реализация, но она позволяет обрабатывать сложные выражения, не прибегая к алгоритму рекурсивного спуска для правильного считывания правил. Также это позволяет избежать многократных считываний из ввода.

Пример трансляции:

`((!a & b) | c) ? a = 0, b = 1;` транслируется в 
```
if ((!variables->data[0].value && variables->data[1].value) || variables->data[2].value) {
variables->data[0].value = 0;
variables->data[1].value = 1;

is_change = 1;
fprintf(report, "the rule № 1 worked\n");
}
```

У данного подхода есть минус: пришлось разделить программу на 2 части(генерацию правил и их выполнение). Так как не получилось реализовать единую версию. 

На мой взгляд лаконичность и простота этого подхода перекрывает данный минус.


# Ответы на допвопросы:
1. 
* Выявлять правила, которые никогда не будут срабатывать и удалять их.
* Объединить части программы в одну, так как некоторые действия происходят по 2 раза.

2. Некоторая неопределенность может возникать в случае противоречащих друг другу правил: ```(a) ? b = 0;
(a) ? b = 1;```
Но в таком случае будет главным правило, которое идет последним, так что неопределенностей быть не может.

3. Машина может не остановиться, если в системе присутствуют циклические зависимости. Например:

Правило 1: Если A, то B = True
Правило 2: Если B, то A = True

Решения:
* Ввести ограничение на максимальное количество циклов.
* Предусмотреть механизм обнаружения циклических зависимостей.