--TEST--
Tests Argon2i hash and verify
--FILE--
<?php
$hash = argon2_hash('password', HASH_ARGON2I);
var_dump(argon2_verify('password', $hash));
var_dump(argon2_verify('badpass', $hash));
--EXPECT--
bool(true)
bool(false)