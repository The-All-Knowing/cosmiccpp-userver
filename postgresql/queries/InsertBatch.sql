INSERT INTO allocation.batches (reference, sku, _purchased_quantity, eta)
VALUES ($1, $2, $3, $4)
RETURNING id;