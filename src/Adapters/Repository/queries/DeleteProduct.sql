DELETE FROM allocation.products
WHERE sku = $1 AND version_number = $2;