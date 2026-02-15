INSERT INTO allocation.order_lines (sku, qty, orderid)
VALUES ($1, $2, $3)
RETURNING id;