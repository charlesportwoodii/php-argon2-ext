--TEST--
Tests Argon2 exceptions
--FILE--
<?php
try {
    $hash = argon2_hash('test', ARGON2_PASSWORD, ['m_cost' => 0]);
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}

try {
    $hash = argon2_hash('test', ARGON2_PASSWORD, ['t_cost' => 0]);
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}

try {
    $hash = argon2_hash('test', ARGON2_PASSWORD, ['threads' => 0]);
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}
--EXPECT--
string(24) "Memory cost is too small"
string(22) "Time cost is too small"
string(13) "Too few lanes"