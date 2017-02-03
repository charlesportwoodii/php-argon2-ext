--TEST--
Tests Argon2 constants are defined
--FILE--
<?php
var_dump(HASH_ARGON2);
var_dump(HASH_ARGON2ID);
var_dump(HASH_ARGON2I);
var_dump(HASH_ARGON2D);
--EXPECT--
int(2)
int(2)
int(1)
int(0)