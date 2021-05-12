# anemone-1

[ENGLISH LANGUAGE]

The Anemone-1 block cipher was created as a demonstration example to show the approximate structure of block ciphers constructed from the Feistel network. The encryption key generation function uses a simple scheme of filling the key table with values ​​from 0 to 256, and then rearranges these values ​​depending on the key and its length. The cipher itself is built on a simple operation of adding a 32-bit plaintext word with a 32-bit encryption key word, followed by replacing the right side of the plaintext with left and left with right, with many permutations. The cipher generates good randomness, but no one has researched it! Do not use it to encrypt sensitive data. 

[РУССКИЙ ЯЗЫК]

Блочный шифр Анемона-1 создан в качестве демонстрационно примера, чтобы показать примерную структуру блочных шифров, сконструированных на основе сети Фейстеля. Функция генерации ключа шифрования использует простую схему заполнения ключевой таблицы значениями от 0 до 256, а после переставляет эти значения в зависимости от ключа и его длины. Сам шифр построен на простой операции сложения 32-битного слова открытого текста с 32-битным словом ключа шифрования, с последующей заменой правой части открытого текста на левую и левой на правую, с множеством перестановок. Шифр порождает хорошую случайность, но его никто не исследовал! Не используйте его для шифрования важных данных.
