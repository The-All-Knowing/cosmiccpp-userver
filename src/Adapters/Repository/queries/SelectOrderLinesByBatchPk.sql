SELECT l.sku, l.qty, l.orderid
FROM allocation.order_lines l
JOIN allocation.allocations o ON l.id = o.orderline_id
WHERE o.batch_id = $1;