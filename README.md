# Терморегулятор на Arduino

Изначально проект был рассчитан для лабораторного блока питания.
В ходе разработки кода были добавлены функции управления по Bluetooth и получения температуры на смартфон.
Код может быть использован для управления любых устройств.

- sendBluetooth() - Отправляет переменные с Arduino по Bluetooth. Переменные должны отправляться через разделительный символ. 
- getBluetooth() - Принимает данные по Bluetooth на Arduino. Данные принимаются с префиксом команды - им может быть любая буква, и суффиксом перевода строки "CR" или "\n".


Представленная схема для текущего проекта термоконтроллера для лабораторного блока питания.
![Схема](https://github.com/ELCORONE/Thermocontrol/blob/master/pcb.png)
