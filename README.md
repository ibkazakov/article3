Содержимое:
1. /detector/detector
2. /transmitter/transmitter
Как передать файл:
1. Запустить transitter. При этом transmitter выведет свой собственный PID
2. Запустить detector. Ввести в консоль PID transmitter.
3. Ввести в консоль transmitter имя передаемого файла.(т.е. путь к нему)
4. Ввести в консоль detector имя файла, в который передаемый файл предполагается копировать.
5. Начать работу detector (финальная стадия, нажать 0)
6. Начать работу transmitter.

По ходу передачи transmitter будет циклически печатать номер отправленного блока. 
Detector печает номер полученного блока и его состояние: успешно\неуспешно\уже был принят раньше.
Успешно принятый блок сохраняется в временном файле. Когда все блоки становятся приняты, detector собирает из
этих временных файлов файл-результат.

Как и было указано в тексте статьи, скрытая передача осуществляется посредством изменение передачиком своего параметра VmData, и считыванием приемником этого параметра. "Запись" и "чтение" происходят через соответсвенно определенные интервалы.

В папке error_measure лежит исходный код, относящийся к методике измерения вероятности ошибок.
