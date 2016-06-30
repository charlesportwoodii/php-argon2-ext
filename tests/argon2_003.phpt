--TEST--
Tests Argon2i hash and verify
--FILE--
<?php
$hash = argon2_hash('password', PASSWORD_ARGON2I);
var_dump(argon2_verify('password', $hash));
--EXPECT--
bool(true)