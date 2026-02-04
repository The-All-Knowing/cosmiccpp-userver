SELECT p.sku, p.version_number
FROM allocation.products p
JOIN allocation.batches b ON p.sku = b.sku
WHERE b.reference = $1
LIMIT 1;