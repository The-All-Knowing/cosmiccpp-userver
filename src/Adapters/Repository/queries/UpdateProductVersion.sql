UPDATE allocation.products
SET version_number = $1
WHERE sku = $2 AND version_number = $3;