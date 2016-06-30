--TEST--
Tests Argon2 low memory RuntimeException
--FILE--
<?php
try {
    $hash = argon2_hash('test', PASSWORD_ARGON2I, ['m_cost' => 0]);
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}

try {
    $hash = argon2_hash('test', PASSWORD_ARGON2D, ['m_cost' => 0]);
} catch (RuntimeException $e) {
    var_dump($e->getMessage());
}
--EXPECT--
string(24) "Memory cost is too small"
string(24) "Memory cost is too small"